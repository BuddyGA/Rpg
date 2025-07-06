#include "RpgRenderWorldSubsystem.h"
#include "RpgRenderComponent.h"
#include "RpgRenderer.h"



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
}


void RpgRenderWorldSubsystem::Render(int frameIndex, RpgRenderer* renderer) noexcept
{

#ifndef RPG_BUILD_SHIPPING
	if (bDebugDrawMeshBounds)
	{
		RpgWorld* world = GetWorld();
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
