#pragma once

#include "core/world/RpgComponent.h"
#include "RpgModel.h"



struct RpgRenderComponent_Camera
{
	RPG_COMPONENT_TYPE(1, "RenderComponent_Camera");

public:
	RpgPointInt RenderTargetDimension{ 1600, 900 };
	RpgRenderProjectionMode ProjectionMode{ RpgRenderProjectionMode::PERSPECTIVE };
	float PerspectiveFoVDegree{ 90.0f };
	float NearClipZ{ 10.0f };
	float FarClipZ{ 10000.0f };
	bool bFrustumCulling{ false };


public:
	inline void Reset() noexcept
	{
		RenderTargetDimension = RpgPointInt(1600, 900);
		ProjectionMode = RpgRenderProjectionMode::PERSPECTIVE;
		PerspectiveFoVDegree = 90.0f;
		NearClipZ = 10.0f;
		FarClipZ = 10000.0f;
		bFrustumCulling = false;
	}

};



struct RpgRenderComponent_Mesh
{
	RPG_COMPONENT_TYPE(2, "RenderComponent_Mesh");

public:
	RpgBoundingAABB Bound;
	RpgSharedModel Model;
	bool bIsVisible{ false };


public:
	inline void Reset() noexcept
	{
		Bound = RpgBoundingAABB(RpgVector3(-32.0f), RpgVector3(32.0f));
		Model.Release();
		bIsVisible = false;
	}

};



struct RpgRenderComponent_Light
{
	RPG_COMPONENT_TYPE(3, "RenderComponent_Light");

public:
	// Light type (point light, spot light, directional light)
	RpgRenderLightType Type{ RpgRenderLightType::NONE };

	// Light color and intensity
	// (RGB: color, A: intensity)
	RpgColorLinear ColorIntensity{ 1.0f, 1.0f, 1.0f, 1.0f };

	// For point/spot light only, attenuation radius
	float AttenuationRadius{ 800.0f };

	// For point/spot light only, attenuation falloff exponent
	float AttenuationFallOffExp{ 8.0f };

	// For spotlight only, inner cone (umbra) in degree
	float SpotInnerConeDegree{ 20.0f };

	// For spotlight only, outer cone (penumbra) in degree
	float SpotOuterConeDegree{ 40.0f };

	// Shadow texture dimension. Set 0 to disable cast shadow
	uint16_t ShadowTextureDimension{ 0 };

	// TRUE if light is visible
	bool bIsVisible{ false };


public:
	inline void Reset() noexcept
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

};
