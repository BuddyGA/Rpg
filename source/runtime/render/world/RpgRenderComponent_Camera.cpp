#include "RpgRenderComponent.h"
#include "../RpgSceneViewport.h"



RpgRenderComponent_Camera::RpgRenderComponent_Camera() noexcept
{
	RenderTargetDimension = RpgPointInt(1600, 900);
	ProjectionMode = RpgRenderProjectionMode::PERSPECTIVE;
	PerspectiveFoVDegree = 90.0f;
	NearClipZ = 10.0f;
	FarClipZ = 10000.0f;
	bActivated = false;
	bFrustumCulling = false;
	SceneViewport = nullptr;
}


void RpgRenderComponent_Camera::Destroy() noexcept
{
	if (SceneViewport)
	{
		delete SceneViewport;
		SceneViewport = nullptr;
	}
}
