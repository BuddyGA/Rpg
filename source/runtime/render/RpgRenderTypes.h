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



enum class RpgRenderLightType : uint8_t
{
	NONE = 0,
	POINT_LIGHT,
	SPOT_LIGHT,
	DIRECTIONAL_LIGHT
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
