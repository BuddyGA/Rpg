#pragma once

#include "RpgRenderResource.h"



class RpgShadowViewport : public RpgRenderViewport
{
private:
	struct FFrameData
	{
		RpgSharedTexture2D DepthTexture;
		RpgArray<RpgDrawIndexed> DrawIndexeds;
	};
	FFrameData FrameDatas[RPG_FRAME_BUFFERING];

public:
	RpgPointInt RenderTargetDimension;


public:
	RpgShadowViewport() noexcept;
	~RpgShadowViewport() noexcept;

	virtual void PreRender(int frameIndex, RpgMaterialResource* materialResource, RpgMeshResource* meshResource, RpgMeshSkinnedResource* meshSkinnedResource, RpgWorldResource* worldResource, const RpgWorld* world) noexcept override;
	virtual void SetupRenderPasses(int frameIndex, RpgAsyncTask_RenderPassArray& out_RenderPasses, const RpgMaterialResource* materialResource, const RpgMeshResource* meshResource, const RpgMeshSkinnedResource* meshSkinnedResource, const RpgWorldResource* worldResource) noexcept override;


	[[nodiscard]] inline const RpgSharedTexture2D& GetDepthStencilTexture(int frameIndex) const noexcept
	{
		return FrameDatas[frameIndex].DepthTexture;
	}

};
