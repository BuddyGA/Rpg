#include "RpgShadowViewport.h"



RpgShadowViewport::RpgShadowViewport() noexcept
{
}


RpgShadowViewport::~RpgShadowViewport() noexcept
{
}


void RpgShadowViewport::PreRender(int frameIndex, RpgMaterialResource* materialResource, RpgMeshResource* meshResource, RpgMeshSkinnedResource* meshSkinnedResource, RpgWorldResource* worldResource, const RpgWorld* world) noexcept
{
	FFrameData& frame = FrameDatas[frameIndex];

	// Resize depth texture
	if (!frame.DepthTexture)
	{
		frame.DepthTexture = RpgTexture2D::s_CreateSharedDepthStencil("TEXDS_ShadowViewport", RpgTextureFormat::TEX_DS_16, RenderTargetDimension.X, RenderTargetDimension.Y);
	}

	frame.DepthTexture->Resize(RenderTargetDimension.X, RenderTargetDimension.Y);
	frame.DepthTexture->GPU_UpdateResource();
}


void RpgShadowViewport::SetupRenderPasses(int frameIndex, RpgAsyncTask_RenderPassArray& out_RenderPasses, const RpgMaterialResource* materialResource, const RpgMeshResource* meshResource, const RpgMeshSkinnedResource* meshSkinnedResource, const RpgWorldResource* worldResource) noexcept
{
}
