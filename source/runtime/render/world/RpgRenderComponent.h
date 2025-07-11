#pragma once

#include "core/world/RpgComponent.h"
#include "../RpgModel.h"
#include "../RpgShadowViewport.h"



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

private:
	RpgUniquePtr<RpgShadowViewport> ShadowViewport;


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


	inline RpgShadowViewport* GetShadowViewport() noexcept
	{
		return ShadowViewport.Get();
	}


	friend class RpgRenderWorldSubsystem;

};
