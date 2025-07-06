#pragma once

#include "core/world/RpgComponent.h"
#include "RpgCollisionTypes.h"



struct RpgCollisionComponent_Primitive
{
	RPG_COMPONENT_TYPE(0, "CollisionComponent_Primitive");

private:
	// Internal bounding AABB for broadphase
	RpgBoundingAABB Bounding;

	// - Sphere (X = Radius, Y = Radius, Z = Radius, W = Radius)
	// - Box (XYZ = Half Extents, W = 0.0f)
	// - Capsule (X = Radius, Y = HalfHeight, Z = 0.0f, W = 0.0f)
	RpgVector4 Size;

	// Collision shape
	RpgCollisionShape::EType Shape{ RpgCollisionShape::NONE };

	// Set true to update internal bounding AABB
	bool bUpdateBounding{ false };

public:
	// Set true to enable collision
	bool bCollisionEnabled{ false };

	// Object collision channel
	RpgCollisionChannel::EType ObjectChannel{ RpgCollisionChannel::NONE };

	// Object response againts other channel
	RpgCollisionResponseChannels ResponseChannels{ k_CollisionResponseChannels_IgnoreAll };


public:
	inline void Reset() noexcept
	{
		Bounding = RpgBoundingAABB(RpgVector3(-4.0f), RpgVector3(4.0f));
		Size = RpgVector4();
		Shape = RpgCollisionShape::NONE;
		bUpdateBounding = false;
		bCollisionEnabled = false;
		ObjectChannel = RpgCollisionChannel::NONE;
		ResponseChannels = k_CollisionResponseChannels_IgnoreAll;
	}


	inline void SetShapeAsSphere(float radius) noexcept
	{
		Size = RpgVector4(radius);
		Shape = RpgCollisionShape::SPHERE;
		bUpdateBounding = true;
	}


	inline void SetShapeAsBox(RpgVector3 halfExtents) noexcept
	{
		Size = RpgVector4(halfExtents.X, halfExtents.Y, halfExtents.Z, 0.0f);
		Shape = RpgCollisionShape::BOX;
		bUpdateBounding = true;
	}


	inline void SetShapeAsCapsule(float radius, float halfHeight) noexcept
	{
		Size = RpgVector4(radius, halfHeight, 0.0f, 0.0f);
		Shape = RpgCollisionShape::CAPSULE;
		bUpdateBounding = true;
	}


	friend class RpgCollisionWorldSubsystem;

};
