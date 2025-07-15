#pragma once

#include "RpgRenderResource.h"
#include "task/RpgRenderTask_RenderPass.h"



class RpgShadowViewport
{
	RPG_NOCOPY(RpgShadowViewport)

public:
	// Game object id of this point light
	RpgGameObjectID GameObject;

	// Attenuation radius
	float AttenuationRadius;

	float SpotInnerConeDegree;
	float SpotOuterConeDegree;


public:
	RpgShadowViewport() noexcept = default;
	virtual ~RpgShadowViewport() noexcept = default;
	virtual void PreRender(RpgRenderFrameContext& frameContext, RpgWorldResource* worldResource, const RpgWorld* world, RpgWorldResource::FLightID lightId) noexcept = 0;
	virtual void SetupRenderPasses(const RpgRenderFrameContext& frameContext, const RpgWorldResource* worldResource, const RpgWorld* world, RpgRenderTask_RenderPassShadowArray& out_ShadowPasses) noexcept = 0;
	virtual RpgSharedTexture2D GetDepthTexture(int frameIndex) noexcept = 0;

};



class RpgShadowViewport_PointLight : public RpgShadowViewport
{
private:
	struct FViewInfo
	{
		RpgMatrixTransform ViewMatrix;
		RpgWorldResource::FViewID ViewId{ RPG_INDEX_INVALID };
	};

	// [0: +X] [1: -X] [2: +Y] [3: -Y] [4: +Z] [5: -Z]
	FViewInfo FaceViews[6];


	struct FFrameData
	{
		RpgSharedTextureCubeDepth DepthTextureCube;
		RpgArray<RpgDrawIndexedDepth> DrawMeshes;
		RpgArray<RpgDrawIndexedDepth> DrawSkinnedMeshes;
		RpgRenderTask_RenderPassShadow TaskRenderPassShadow;
	};
	FFrameData FrameDatas[RPG_FRAME_BUFFERING];


public:
	RpgShadowViewport_PointLight() noexcept;
	~RpgShadowViewport_PointLight() noexcept;

	virtual void PreRender(RpgRenderFrameContext& frameContext, RpgWorldResource* worldResource, const RpgWorld* world, RpgWorldResource::FLightID lightId) noexcept override;
	virtual void SetupRenderPasses(const RpgRenderFrameContext& frameContext, const RpgWorldResource* worldResource, const RpgWorld* world, RpgRenderTask_RenderPassShadowArray& out_ShadowPasses) noexcept override;
	
	
	virtual RpgSharedTexture2D GetDepthTexture(int frameIndex) noexcept override
	{
		return FrameDatas[frameIndex].DepthTextureCube.Cast<RpgTexture2D>();
	}

};



class RpgShadowViewport_SpotLight : public RpgShadowViewport
{
private:
	RpgWorldResource::FViewID ViewId;


	struct FFrameData
	{
		RpgSharedTexture2D DepthTexture;
		RpgArray<RpgDrawIndexedDepth> DrawMeshes;
		RpgArray<RpgDrawIndexedDepth> DrawSkinnedMeshes;
		RpgRenderTask_RenderPassShadow TaskRenderPassShadow;
	};
	FFrameData FrameDatas[RPG_FRAME_BUFFERING];


public:
	RpgShadowViewport_SpotLight() noexcept;

	virtual void PreRender(RpgRenderFrameContext& frameContext, RpgWorldResource* worldResource, const RpgWorld* world, RpgWorldResource::FLightID lightId) noexcept override;
	virtual void SetupRenderPasses(const RpgRenderFrameContext& frameContext, const RpgWorldResource* worldResource, const RpgWorld* world, RpgRenderTask_RenderPassShadowArray& out_ShadowPasses) noexcept override;


	virtual RpgSharedTexture2D GetDepthTexture(int frameIndex) noexcept override
	{
		return FrameDatas[frameIndex].DepthTexture;
	}

};
