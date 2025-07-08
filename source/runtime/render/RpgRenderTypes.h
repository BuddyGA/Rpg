#pragma once

#include "RpgD3D12.h"


class RpgWorld;


class RpgRenderer;
class RpgRenderer2D;

class RpgMaterialResource;
class RpgMeshResource;
class RpgMeshSkinnedResource;
class RpgWorldResource;

struct RpgDrawIndexed;

class RpgRenderViewport;
class RpgSceneViewport;
class RpgShadowViewport;

class RpgAsyncTask_CompilePSO;
class RpgAsyncTask_Compute;
class RpgAsyncTask_Copy;
class RpgAsyncTask_RenderPass;
class RpgAsyncTask_RenderPass_Depth;
class RpgAsyncTask_RenderPass_Forward;
class RpgAsyncTask_RenderPass_Transparency;

typedef RpgArrayInline<RpgAsyncTask_RenderPass*, 32> RpgAsyncTask_RenderPassArray;


class RpgAsyncTask_Compute;




enum class RpgRenderProjectionMode : uint8_t
{
	PERSPECTIVE = 0,
	ORTHOGRAPHIC
};



namespace RpgRenderLight
{
	enum EType : uint8_t
	{
		TYPE_NONE = 0,
		TYPE_POINT_LIGHT,
		TYPE_SPOT_LIGHT,
		TYPE_DIRECTIONAL_LIGHT
	};


	enum EShadowResolutionQuality : uint8_t
	{
		SHADOW_RESOLUTION_QUALITY_LOW = 0,
		SHADOW_RESOLUTION_QUALITY_MEDIUM,
		SHADOW_RESOLUTION_QUALITY_HIGH,
		SHADOW_RESOLUTION_QUALITY_MAX_COUNT
	};


	constexpr uint16_t POINT_SHADOW_RESOLUTIONS[SHADOW_RESOLUTION_QUALITY_MAX_COUNT] =
	{
		256,	// LOW
		512,	// MEDIUM
		1024	// HIGH
	};


	constexpr uint16_t SPOT_SHADOW_RESOLUTIONS[SHADOW_RESOLUTION_QUALITY_MAX_COUNT] =
	{
		256,	// LOW
		512,	// MEDIUM
		1024	// HIGH
	};


	constexpr uint16_t DIRECTIONAL_SHADOW_RESOLUTIONS[SHADOW_RESOLUTION_QUALITY_MAX_COUNT] =
	{
		512,	// LOW
		1024,	// MEDIUM
		2048	// HIGH
	};

};



class RpgRenderViewport
{
	RPG_NOCOPY(RpgRenderViewport)

public:
	RpgRenderViewport() noexcept = default;
	virtual ~RpgRenderViewport() noexcept = default;

	virtual void PreRender(int frameIndex, RpgMaterialResource* materialResource, RpgMeshResource* meshResource, RpgMeshSkinnedResource* meshSkinnedResource, RpgWorldResource* worldResource, const RpgWorld* world) noexcept = 0;
	virtual void SetupRenderPasses(int frameIndex, RpgAsyncTask_RenderPassArray& out_RenderPasses, const RpgMaterialResource* materialResource, const RpgMeshResource* meshResource, const RpgMeshSkinnedResource* meshSkinnedResource, const RpgWorldResource* worldResource) noexcept = 0;

};
