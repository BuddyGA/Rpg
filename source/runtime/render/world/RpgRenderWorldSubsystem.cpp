#include "RpgRenderWorldSubsystem.h"
#include "RpgRenderComponent.h"
#include "../RpgSceneViewport.h"
#include "../RpgRenderer.h"



RpgRenderWorldSubsystem::RpgRenderWorldSubsystem() noexcept
{
	Name = "RenderWorldSubsystem";
	bDebugDrawMeshBounds = false;
}


void RpgRenderWorldSubsystem::PostTickUpdate() noexcept
{
    RpgWorld* world = GetWorld();

    for (auto it = world->Component_CreateIterator<RpgRenderComponent_Mesh>(); it; ++it)
    {
        RpgRenderComponent_Mesh& comp = it.GetValue();

        if (!world->GameObject_IsTransformUpdated(comp.GameObject))
        {
            continue;
        }

        comp.Bound = comp.Model ? comp.Model->GetBound() : RpgBoundingAABB(RpgVector3(-32.0f), RpgVector3(32.0f));

        // transform bound into world space
        comp.Bound = RpgBoundingBox(comp.Bound, world->GameObject_GetWorldTransformMatrix(comp.GameObject)).ToAABB();
    }


	for (auto it = world->Component_CreateIterator<RpgRenderComponent_Camera>(); it; ++it)
	{
		RpgRenderComponent_Camera& comp = it.GetValue();
		if (!comp.bActivated)
		{
			continue;
		}

		if (comp.SceneViewport == nullptr)
		{
			comp.SceneViewport = new RpgSceneViewport();
		}

		RpgSceneViewport* sceneViewport = comp.SceneViewport;
		sceneViewport->RenderTargetDimension = comp.RenderTargetDimension;
		
		const RpgTransform worldTransform(world->GameObject_GetWorldTransformMatrix(comp.GameObject));
		sceneViewport->SetCameraRotationAndPosition(worldTransform.Rotation, worldTransform.Position);

		if (comp.ProjectionMode == RpgRenderProjectionMode::PERSPECTIVE)
		{
			sceneViewport->SetProjectionPerspective(comp.PerspectiveFoVDegree, comp.NearClipZ, comp.FarClipZ);
		}
		else
		{
			sceneViewport->SetProjectionOrthographic(comp.NearClipZ, comp.FarClipZ);
		}

		sceneViewport->bFrustumCulling = comp.bFrustumCulling;
	}
}


void RpgRenderWorldSubsystem::Render(int frameIndex, RpgRenderer* renderer) noexcept
{
	const RpgWorld* world = GetWorld();

	for (auto it = world->Component_CreateConstIterator<RpgRenderComponent_Camera>(); it; ++it)
	{
		const RpgRenderComponent_Camera& comp = it.GetValue();
		if (!comp.bActivated)
		{
			continue;
		}

		renderer->AddWorldViewport(frameIndex, world, comp.SceneViewport);
	}


#ifndef RPG_BUILD_SHIPPING
	if (bDebugDrawMeshBounds)
	{
		RpgVertexPrimitiveBatchLine* debugLine = renderer->Debug_GetPrimitiveBatchLine(frameIndex, world, false);

		for (auto it = world->Component_CreateConstIterator<RpgRenderComponent_Mesh>(); it; ++it)
		{
			const RpgRenderComponent_Mesh& comp = it.GetValue();
			if (!comp.bIsVisible)
			{
				continue;
			}

			debugLine->AddAABB(comp.Bound, RpgColorRGBA(255, 255, 255));
		}
	}
#endif // RPG_BUILD_SHIPPING

}
