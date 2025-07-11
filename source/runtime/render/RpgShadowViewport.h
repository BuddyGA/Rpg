#pragma once

#include "RpgRenderResource.h"
#include "async_task/RpgAsyncTask_RenderPass.h"



class RpgShadowViewport
{
	RPG_NOCOPY(RpgShadowViewport)

public:
	// Game object id of this point light
	RpgGameObjectID GameObject;

	// Attenuation radius
	float AttenuationRadius;


public:
	RpgShadowViewport() noexcept = default;
	virtual ~RpgShadowViewport() noexcept = default;
	virtual void PreRender(RpgRenderFrameContext& frameContext, RpgWorldResource* worldResource, const RpgWorld* world, const RpgWorldResource::FLightID* lightIds, int lightCount) noexcept = 0;
	virtual void SetupRenderPasses(const RpgRenderFrameContext& frameContext, const RpgWorldResource* worldResource, const RpgWorld* world, RpgAsyncTask_RenderPassShadowArray& out_ShadowPasses) noexcept = 0;
	virtual RpgSharedTexture2D GetDepthTexture(int frameIndex) noexcept = 0;

};



class RpgShadowViewport_PointLight : public RpgShadowViewport
{
private:
	struct FViewInfo
	{
		RpgMatrixTransform ViewMatrix;
		RpgMatrixProjection ProjectionMatrix;
		float FarClipZ{ 0.0f };
		RpgWorldResource::FCameraID CameraId{ RPG_INDEX_INVALID };
		int ShadowTextureDescriptorIndex{ RPG_INDEX_INVALID };
	};

	// [0: +X] [1: -X] [2: +Y] [3: -Y] [4: +Z] [5: -Z]
	FViewInfo FaceViews[6];


	struct FFrameData
	{
		RpgSharedTexture2D DepthTextures[6];
		RpgArray<RpgDrawIndexedDepth> DrawMeshes;
		RpgArray<RpgDrawIndexedDepth> DrawSkinnedMeshes;
		RpgAsyncTask_RenderPass_Shadow AsyncTaskRenderPassShadow;
	};
	FFrameData FrameDatas[RPG_FRAME_BUFFERING];


public:
	RpgShadowViewport_PointLight() noexcept;
	~RpgShadowViewport_PointLight() noexcept;

	virtual void PreRender(RpgRenderFrameContext& frameContext, RpgWorldResource* worldResource, const RpgWorld* world, const RpgWorldResource::FLightID* lightIds, int lightCount) noexcept override;
	virtual void SetupRenderPasses(const RpgRenderFrameContext& frameContext, const RpgWorldResource* worldResource, const RpgWorld* world, RpgAsyncTask_RenderPassShadowArray& out_ShadowPasses) noexcept override;
	
	
	virtual RpgSharedTexture2D GetDepthTexture(int frameIndex) noexcept override
	{
		return FrameDatas[frameIndex].DepthTextures[0].Cast<RpgTexture2D>();
	}

};
