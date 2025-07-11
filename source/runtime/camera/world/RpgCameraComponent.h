#pragma once

#include "core/world/RpgComponent.h"
#include "render/RpgSceneViewport.h"
#include "render/RpgShadowViewport.h"
#include "../RpgCameraTypes.h"



class RpgCameraComponent
{
	RPG_COMPONENT_TYPE("RpgComponent - Camera");

public:
	RpgPointInt RenderTargetDimension;
	RpgCameraProjectionMode ProjectionMode;
	float PerspectiveFoVDegree;
	float NearClipZ;
	float FarClipZ;
	bool bActivated;
	bool bFrustumCulling;

	RpgSceneViewport* Viewport;

private:
	RpgUniquePtr<RpgSceneViewport> SelfViewport;


public:
	RpgCameraComponent() noexcept
	{
		RenderTargetDimension = RpgPointInt(1600, 900);
		ProjectionMode = RpgCameraProjectionMode::PERSPECTIVE;
		PerspectiveFoVDegree = 90.0f;
		NearClipZ = 10.0f;
		FarClipZ = 10000.0f;
		bActivated = false;
		bFrustumCulling = false;
		Viewport = nullptr;
	}


	inline void Destroy() noexcept
	{
		// Nothing to do
	}


	inline RpgSceneViewport* GetSceneViewport() noexcept
	{
		if (Viewport)
		{
			return Viewport;
		}

		if (!SelfViewport)
		{
			SelfViewport = RpgPointer::MakeUnique<RpgSceneViewport>();
		}

		return SelfViewport.Get();
	}


	friend RpgCameraWorldSubsystem;
	friend RpgAsyncTask_CaptureMesh;
	friend RpgAsyncTask_CaptureLight;

};
