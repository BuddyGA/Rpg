#pragma once

#include "core/world/RpgComponent.h"
#include "../RpgModel.h"
#include "../RpgSceneViewport.h"




class RpgRenderComponent_Camera
{
	RPG_COMPONENT_TYPE("RpgComponent - Camera");

public:
	RpgPointInt RenderTargetDimension;
	RpgRenderProjectionMode ProjectionMode;
	float PerspectiveFoVDegree;
	float NearClipZ;
	float FarClipZ;
	bool bActivated;
	bool bFrustumCulling;

	RpgGameObjectID SourceCaptureCamera;
	RpgSceneViewport* Viewport;

private:
	RpgSceneViewport SelfViewport;


public:
	RpgRenderComponent_Camera() noexcept
	{
		RenderTargetDimension = RpgPointInt(1600, 900);
		ProjectionMode = RpgRenderProjectionMode::PERSPECTIVE;
		PerspectiveFoVDegree = 90.0f;
		NearClipZ = 10.0f;
		FarClipZ = 10000.0f;
		bActivated = false;
		bFrustumCulling = false;
	}


	inline void Destroy() noexcept
	{
		// Nothing to do
	}


	inline RpgSceneViewport* GetViewport() noexcept
	{
		return Viewport ? Viewport : &SelfViewport;
	}

	

	friend class RpgRenderWorldSubsystem;

};



class RpgRenderComponent_Mesh
{
	RPG_COMPONENT_TYPE("RpgComponent - Mesh");

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


	friend class RpgRenderWorldSubsystem;

};



class RpgRenderComponent_Light
{
	RPG_COMPONENT_TYPE("RpgComponent - Light");

public:
	// Light type (point light, spot light, directional light)
	RpgRenderLight::EType Type;

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

	// TRUE if light visible
	bool bIsVisible;

	// TRUE if light cast shadow
	bool bCastShadow;


public:
	RpgRenderComponent_Light() noexcept
	{
		Type = RpgRenderLight::TYPE_NONE;
		ColorIntensity = RpgColorLinear(1.0f, 1.0f, 1.0f, 1.0f);
		AttenuationRadius = 800.0f;
		AttenuationFallOffExp = 8.0f;
		SpotInnerConeDegree = 20.0f;
		SpotOuterConeDegree = 40.0f;
		bIsVisible = false;
		bCastShadow = false;
	}


	inline void Destroy() noexcept
	{
		// Nothing to do
	}


	friend class RpgRenderWorldSubsystem;

};
