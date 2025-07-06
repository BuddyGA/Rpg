#include "RpgCollisionWorldSubsystem.h"
#include "RpgCollisionComponent.h"
#include "thirdparty/libccd/ccd.h"



#define RPG_LIBCCD_GJK_MAX_ITERATIONS		(32)
#define RPG_LIBCCD_GJK_MAX_EPA_TOLERANCE	(0.001f)
#define RPG_LIGCCD_GJK_DISTANCE_TOLERANCE	(0.001f)



namespace RpgCollisionGJK
{
	static void SupportSphere(const void* obj, const ccd_vec3_t* dir, ccd_vec3_t* vec) noexcept
	{
		const RpgBoundingSphere* sphere = reinterpret_cast<const RpgBoundingSphere*>(obj);
		const RpgVector3 direction(dir->v[0], dir->v[1], dir->v[2]);
		const RpgVector3 farthestPoint = sphere->Center + direction.GetNormalize() * sphere->Radius;

		ccdVec3Set(vec, farthestPoint.X, farthestPoint.Y, farthestPoint.Z);
	}


	static void SupportBox(const void* obj, const ccd_vec3_t* dir, ccd_vec3_t* vec) noexcept
	{
		const RpgBoundingBox* box = reinterpret_cast<const RpgBoundingBox*>(obj);
		const RpgBoundingBox::FCornerPoints corners = box->GetCornerPoints();
		const RpgVector3 direction(dir->v[0], dir->v[1], dir->v[2]);

		float maxDot = -FLT_MAX;
		int cornerIndex = -1;

		for (int i = 0; i < 8; ++i)
		{
			const float dot = RpgVector3::DotProduct(corners.Points[i], direction);
			if (dot > maxDot)
			{
				maxDot = dot;
				cornerIndex = i;
			}
		}

		RPG_PLATFORM_Check(cornerIndex != -1);
		const RpgVector3 farthestPoint = corners.Points[cornerIndex];

		ccdVec3Set(vec, farthestPoint.X, farthestPoint.Y, farthestPoint.Z);
	}


	static void SupportCapsule(const void* obj, const ccd_vec3_t* dir, ccd_vec3_t* vec) noexcept
	{
		const RpgBoundingCapsule* capsule = reinterpret_cast<const RpgBoundingCapsule*>(obj);
		const RpgVector3 direction(dir->v[0], dir->v[1], dir->v[2]);

		// Capsule always sweep along y-axis
		const RpgVector3 CAPSULE_AXIS(0.0f, 1.0f, 0.0f);

		const float dotAxis = RpgVector3::DotProduct(direction, CAPSULE_AXIS);
		const RpgVector3 axisDir = (dotAxis >= 0.0f) ? CAPSULE_AXIS : CAPSULE_AXIS * -1.0f;
		const RpgVector3 capCenter = capsule->Center + axisDir * capsule->HalfHeight;
		const RpgVector3 farthestPoint = capCenter + direction.GetNormalize() * capsule->Radius;

		ccdVec3Set(vec, farthestPoint.X, farthestPoint.Y, farthestPoint.Z);
	}

};



RpgCollisionWorldSubsystem::RpgCollisionWorldSubsystem() noexcept
{
	Name = "CollisionWorldSubsystem";
	bTickUpdateCollision = false;
}


void RpgCollisionWorldSubsystem::StartPlay() noexcept
{
	bTickUpdateCollision = true;
}


void RpgCollisionWorldSubsystem::StopPlay() noexcept
{
	bTickUpdateCollision = false;
}


void RpgCollisionWorldSubsystem::TickUpdate(float deltaTime) noexcept
{
	if (!bTickUpdateCollision)
	{
		return;
	}

	RpgWorld* world = GetWorld();

	for (auto firstIt = world->Component_CreateIterator<RpgCollisionComponent_Primitive>(); firstIt; ++firstIt)
	{
		RpgCollisionComponent_Primitive& firstCollision = firstIt.GetValue();

		for (auto secondIt = firstIt; secondIt; ++secondIt)
		{
			if (firstIt == secondIt)
			{
				continue;
			}

			const RpgCollisionComponent_Primitive& secondCollision = secondIt.GetValue();

		}
	}
}


bool RpgCollisionWorldSubsystem::TestOverlapSphereSphere(RpgBoundingSphere first, RpgBoundingSphere second, RpgCollisionContactResult* out_Result) noexcept
{
	const RpgVector3 delta = second.Center - first.Center;
	const float magSqr = delta.GetMagnitudeSqr();
	const float radiusSqr = (first.Radius + second.Radius) * (first.Radius * second.Radius);
	const bool bOverlapped = magSqr <= radiusSqr;

	if (bOverlapped && out_Result)
	{
		const RpgVector3 deltaNormalized = delta.GetNormalize();
		out_Result->SeparationDirection = deltaNormalized;
		out_Result->ContactPoint = deltaNormalized * second.Radius;
		out_Result->PenetrationDepth = RpgMath::Sqrt(magSqr);
	}

	return bOverlapped;
}


bool RpgCollisionWorldSubsystem::TestOverlapSphereBox(RpgBoundingSphere sphere, RpgBoundingBox box, RpgCollisionContactResult* out_Result) noexcept
{
	ccd_t ccd;
	CCD_INIT(&ccd);
	ccd.support1 = RpgCollisionGJK::SupportSphere;
	ccd.support2 = RpgCollisionGJK::SupportBox;
	ccd.max_iterations = RPG_LIBCCD_GJK_MAX_ITERATIONS;
	ccd.epa_tolerance = RPG_LIBCCD_GJK_MAX_EPA_TOLERANCE;
	ccd.dist_tolerance = RPG_LIGCCD_GJK_DISTANCE_TOLERANCE;

	bool bOverlapped = false;

	if (out_Result)
	{
		ccd_real_t depth = 0.0f;
		ccd_vec3_t separationDirection;
		ccd_vec3_t contactPoint;

		const int ret = ccdGJKPenetration(&sphere, &box, &ccd, &depth, &separationDirection, &contactPoint);
		RPG_PLATFORM_Check(ret != -2);

		if (ret == 0)
		{
			bOverlapped = true;
			out_Result->SeparationDirection = RpgVector3(separationDirection.v[0], separationDirection.v[1], separationDirection.v[2]);
			out_Result->SeparationDirection.Normalize();
			out_Result->ContactPoint = RpgVector3(contactPoint.v[0], contactPoint.v[1], contactPoint.v[2]);
			out_Result->PenetrationDepth = depth;
		}
	}
	else
	{
		bOverlapped = ccdGJKIntersect(&sphere, &box, &ccd);
	}

	return bOverlapped;
}


bool RpgCollisionWorldSubsystem::TestOverlapBoxBox(RpgBoundingBox first, RpgBoundingBox second, RpgCollisionContactResult* out_Result) noexcept
{
	ccd_t ccd;
	CCD_INIT(&ccd);
	ccd.support1 = RpgCollisionGJK::SupportBox;
	ccd.support2 = RpgCollisionGJK::SupportBox;
	ccd.max_iterations = RPG_LIBCCD_GJK_MAX_ITERATIONS;
	ccd.epa_tolerance = RPG_LIBCCD_GJK_MAX_EPA_TOLERANCE;
	ccd.dist_tolerance = RPG_LIGCCD_GJK_DISTANCE_TOLERANCE;

	return ccdGJKIntersect(&first, &second, &ccd);
}


bool RpgCollisionWorldSubsystem::TestOverlapBoxSphere(RpgBoundingBox box, RpgBoundingSphere sphere, RpgCollisionContactResult* out_Result) noexcept
{
	ccd_t ccd;
	CCD_INIT(&ccd);
	ccd.support1 = RpgCollisionGJK::SupportBox;
	ccd.support2 = RpgCollisionGJK::SupportSphere;
	ccd.max_iterations = RPG_LIBCCD_GJK_MAX_ITERATIONS;
	ccd.epa_tolerance = RPG_LIBCCD_GJK_MAX_EPA_TOLERANCE;
	ccd.dist_tolerance = RPG_LIGCCD_GJK_DISTANCE_TOLERANCE;

	return ccdGJKIntersect(&box, &sphere, &ccd);
}
