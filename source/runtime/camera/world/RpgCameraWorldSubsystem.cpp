#include "RpgCameraWorldSubsystem.h"
#include "RpgCameraComponent.h"
#include "render/RpgRenderer.h"



RpgCameraWorldSubsystem::RpgCameraWorldSubsystem() noexcept
{
}


void RpgCameraWorldSubsystem::PostTickUpdate() noexcept
{
	RpgWorld* world = GetWorld();

	for (auto it = world->Component_CreateIterator<RpgCameraComponent>(); it; ++it)
	{
		RpgCameraComponent& comp = it.GetValue();
		if (!comp.bActivated)
		{
			continue;
		}

		RpgSceneViewport* sceneViewport = comp.GetSceneViewport();
		sceneViewport->RenderTargetDimension = comp.RenderTargetDimension;

		const RpgTransform worldTransform = world->GameObject_GetWorldTransform(comp.GameObject);
		sceneViewport->SetViewRotationAndPosition(worldTransform.Rotation, worldTransform.Position);

		if (comp.ProjectionMode == RpgCameraProjectionMode::PERSPECTIVE)
		{
			sceneViewport->SetProjectionPerspective(comp.PerspectiveFoVDegree, comp.NearClipZ, comp.FarClipZ);
		}
		else
		{
			sceneViewport->SetProjectionOrthographic(comp.NearClipZ, comp.FarClipZ);
		}

		sceneViewport->UpdateViewProjection();

		RpgAsyncTask_CaptureMesh taskCaptureMesh;
		taskCaptureMesh.World = world;
		taskCaptureMesh.Camera = &comp;
		taskCaptureMesh.Execute();

		RpgAsyncTask_CaptureLight taskCaptureLight;
		taskCaptureLight.World = world;
		taskCaptureLight.Camera = &comp;
		taskCaptureLight.Execute();
	}
}


void RpgCameraWorldSubsystem::Render(int frameIndex, RpgRenderer* renderer) noexcept
{
	RpgWorld* world = GetWorld();

	for (auto it = world->Component_CreateIterator<RpgCameraComponent>(); it; ++it)
	{
		RpgCameraComponent& comp = it.GetValue();
		if (!comp.bActivated)
		{
			continue;
		}

		renderer->AddWorldSceneViewport(frameIndex, world, comp.GetSceneViewport());
	}
}
