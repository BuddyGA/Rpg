#pragma once

#include "core/world/RpgComponent.h"
#include "../RpgModel.h"



class RpgRenderComponent_Camera
{
	RPG_COMPONENT_TYPE("Camera");

public:
	RpgPointInt RenderTargetDimension;
	RpgRenderProjectionMode ProjectionMode;
	float PerspectiveFoVDegree;
	float NearClipZ;
	float FarClipZ;
	bool bActivated;
	bool bFrustumCulling;

	RpgGameObjectID SourceCaptureCamera;

private:
	RpgSceneViewport* SceneViewport;


public:
	RpgRenderComponent_Camera() noexcept;
	void Destroy() noexcept;


	friend class RpgRenderWorldSubsystem;

};



class RpgRenderComponent_Mesh
{
	RPG_COMPONENT_TYPE("Mesh");

public:
	RpgBoundingAABB Bound;
	RpgSharedModel Model;
	bool bIsVisible;


public:
	RpgRenderComponent_Mesh() noexcept
	{
		Bound = RpgBoundingAABB(RpgVector3(-32.0f), RpgVector3(32.0f));
		bIsVisible = false;
	}


	inline void Destroy() noexcept
	{
		// Nothing to do
	}

};



class RpgRenderComponent_Light
{
	RPG_COMPONENT_TYPE("Light");

public:
	// Light type (point light, spot light, directional light)
	RpgRenderLightType Type;

	// Light color and intensity
	// (RGB: color, A: intensity)
	RpgColorLinear ColorIntensity;

	// For point/spot light only, attenuation radius
	float AttenuationRadius;

	// For point/spot light only, attenuation falloff exponent
	float AttenuationFallOffExp;

	// For spotlight only, inner cone (umbra) in degree
	float SpotInnerConeDegree;

	// For spotlight only, outer cone (penumbra) in degree
	float SpotOuterConeDegree;

	// Shadow texture dimension. Set 0 to disable cast shadow
	uint16_t ShadowTextureDimension;

	// TRUE if light is visible
	bool bIsVisible;


public:
	RpgRenderComponent_Light() noexcept
	{
		Type = RpgRenderLightType::NONE;
		ColorIntensity = RpgColorLinear(1.0f, 1.0f, 1.0f, 1.0f);
		AttenuationRadius = 800.0f;
		AttenuationFallOffExp = 8.0f;
		SpotInnerConeDegree = 20.0f;
		SpotOuterConeDegree = 40.0f;
		ShadowTextureDimension = 0;
		bIsVisible = false;
	}


	inline void Destroy() noexcept
	{
		// Nothing to do
	}

};
