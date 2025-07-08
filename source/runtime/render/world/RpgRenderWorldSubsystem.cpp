#include "RpgRenderWorldSubsystem.h"
#include "RpgRenderComponent.h"
#include "../RpgSceneViewport.h"
#include "../RpgRenderer.h"



RpgRenderWorldSubsystem::RpgRenderWorldSubsystem() noexcept
{
	Name = "RenderWorldSubsystem";
	bDebugDrawMeshBounds = false;
	bDebugDrawLightBounds = false;
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

		RpgSceneViewport* sceneViewport = comp.GetViewport();
		sceneViewport->RenderTargetDimension = comp.RenderTargetDimension;
		
		const RpgTransform worldTransform = world->GameObject_GetWorldTransform(comp.GameObject);
		sceneViewport->SetViewRotationAndPosition(worldTransform.Rotation, worldTransform.Position);

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
	RpgWorld* world = GetWorld();

	for (auto it = world->Component_CreateIterator<RpgRenderComponent_Camera>(); it; ++it)
	{
		RpgRenderComponent_Camera& comp = it.GetValue();
		if (!comp.bActivated)
		{
			continue;
		}

		renderer->AddWorldViewport(frameIndex, world, comp.GetViewport());
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

	if (bDebugDrawLightBounds)
	{

	}
#endif // RPG_BUILD_SHIPPING

}
