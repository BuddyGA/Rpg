#include "RpgAsyncTask_RenderPass.h"
#include "../RpgRenderPipeline.h"
#include "../RpgRenderResource.h"
#include "../RpgTexture.h"



RpgAsyncTask_RenderPass_Forward::RpgAsyncTask_RenderPass_Forward() noexcept
{
	RenderTargetTexture = nullptr;
	DepthStencilTexture = nullptr;
	DrawMeshData = nullptr;
	DrawMeshCount = 0;
	DrawSkinnedMeshData = nullptr;
	DrawSkinnedMeshCount = 0;

#ifndef RPG_BUILD_SHIPPING
	DebugDrawLineMaterialId = RPG_INDEX_INVALID;
	DebugDrawLineNoDepthMaterialId = RPG_INDEX_INVALID;
	DebugDrawCameraId = RPG_INDEX_INVALID;
#endif // !RPG_BUILD_SHIPPING
}


void RpgAsyncTask_RenderPass_Forward::Reset() noexcept
{
	RpgAsyncTask_RenderPass::Reset();

	RenderTargetTexture = nullptr;
	DepthStencilTexture = nullptr;
	DrawMeshData = nullptr;
	DrawMeshCount = 0;
	DrawSkinnedMeshData = nullptr;
	DrawSkinnedMeshCount = 0;

#ifndef RPG_BUILD_SHIPPING
	DebugDrawLineMaterialId = RPG_INDEX_INVALID;
	DebugDrawLineNoDepthMaterialId = RPG_INDEX_INVALID;
	DebugDrawCameraId = RPG_INDEX_INVALID;
#endif // !RPG_BUILD_SHIPPING
}


void RpgAsyncTask_RenderPass_Forward::CommandDraw(ID3D12GraphicsCommandList* cmdList) const noexcept
{
	RPG_Assert(RenderTargetTexture);
	RPG_Assert(DepthStencilTexture);
	
	const RpgPointInt renderTargetDimension = RenderTargetTexture->GetDimension();

	// Set viewport
	RpgD3D12Command::SetViewport(cmdList, 0, 0, renderTargetDimension.X, renderTargetDimension.Y, 0.0f, 1.0f);

	// Set scissor
	RpgD3D12Command::SetScissor(cmdList, 0, 0, renderTargetDimension.X, renderTargetDimension.Y);

	ID3D12Resource* renderTargetResource = RenderTargetTexture->GPU_GetResource();
	const RpgD3D12::FResourceDescriptor renderTargetDescriptor = RpgD3D12::AllocateDescriptor_RTV(renderTargetResource);

	// Transition resource to render target
	RpgD3D12Command::TransitionAllSubresources(cmdList, renderTargetResource, RenderTargetTexture->GPU_GetState(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	RenderTargetTexture->GPU_SetState(D3D12_RESOURCE_STATE_RENDER_TARGET);

	ID3D12Resource* depthStencilResource = DepthStencilTexture->GPU_GetResource();
	const RpgD3D12::FResourceDescriptor depthStencilDescriptor = RpgD3D12::AllocateDescriptor_DSV(depthStencilResource);

	// Transition resource to depth-write
	RpgD3D12Command::TransitionAllSubresources(cmdList, depthStencilResource, DepthStencilTexture->GPU_GetState(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
	DepthStencilTexture->GPU_SetState(D3D12_RESOURCE_STATE_DEPTH_WRITE);

	// Set and clear render targets
	RpgD3D12Command::SetAndClearRenderTargets(cmdList, &renderTargetDescriptor, 1, RpgColorLinear(0.0f, 0.0f, 0.0f), &depthStencilDescriptor, 1.0f, 0);

	// Bind shader resource material
	FrameContext.MaterialResource->CommandBindShaderResources(cmdList);

	// Bind shader resource world
	WorldResource->CommandBindShaderResources(cmdList);

	// Set topology
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Draw mesh
	if (DrawMeshData)
	{
		RPG_Assert(DrawMeshCount > 0);

		// Bind vertex buffers
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[3] =
		{
			FrameContext.MeshResource->GetVertexBufferView_Position(),
			FrameContext.MeshResource->GetVertexBufferView_NormalTangent(),
			FrameContext.MeshResource->GetVertexBufferView_TexCoord()
		};
		cmdList->IASetVertexBuffers(0, 3, vertexBufferViews);

		// Bind index buffer
		const D3D12_INDEX_BUFFER_VIEW indexBufferView = FrameContext.MeshResource->GetIndexBufferView();
		cmdList->IASetIndexBuffer(&indexBufferView);

		// Draw calls
		for (int d = 0; d < DrawMeshCount; ++d)
		{
			const RpgDrawIndexed& draw = DrawMeshData[d];
			FrameContext.MaterialResource->CommandBindMaterial(cmdList, draw.Material);

			cmdList->SetGraphicsRoot32BitConstants(RpgRenderPipeline::GRPI_OBJECT_PARAM, sizeof(RpgShaderObjectParameter) / 4, &draw.ObjectParam, 0);
			cmdList->DrawIndexedInstanced(draw.IndexCount, 1, draw.IndexStart, draw.IndexVertexOffset, 0);
		}
	}


	// Draw mesh skinned
	if (DrawSkinnedMeshData)
	{
		RPG_Assert(DrawSkinnedMeshCount > 0);

		// Bind vertex buffers
		const D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[3] =
		{
			FrameContext.MeshSkinnedResource->GetVertexBufferView_SkinnedPosition(),
			FrameContext.MeshSkinnedResource->GetVertexBufferView_SkinnedNormalTangent(),
			FrameContext.MeshSkinnedResource->GetVertexBufferView_SkinnedTexCoord()
		};
		cmdList->IASetVertexBuffers(0, 3, vertexBufferViews);

		// Bind index buffer
		const D3D12_INDEX_BUFFER_VIEW indexBufferView = FrameContext.MeshSkinnedResource->GetIndexBufferView_Skinned();
		cmdList->IASetIndexBuffer(&indexBufferView);

		const RpgArray<RpgShaderSkinnedObjectParameter>& skinnedObjectParams = FrameContext.MeshSkinnedResource->GetObjectParameters();
		RPG_Check(skinnedObjectParams.GetCount() == DrawSkinnedMeshCount);

		// Draw calls
		for (int d = 0; d < DrawSkinnedMeshCount; ++d)
		{
			const RpgDrawIndexed& draw = DrawSkinnedMeshData[d];
			FrameContext.MaterialResource->CommandBindMaterial(cmdList, draw.Material);

			const RpgShaderSkinnedObjectParameter& skinnedParam = skinnedObjectParams[d];

			cmdList->SetGraphicsRoot32BitConstants(RpgRenderPipeline::GRPI_OBJECT_PARAM, sizeof(RpgShaderObjectParameter) / 4, &draw.ObjectParam, 0);
			cmdList->DrawIndexedInstanced(skinnedParam.IndexCount, 1, skinnedParam.IndexStart, skinnedParam.VertexStart, 0);
		}
	}

	
// Debug draw
#ifndef RPG_BUILD_SHIPPING
	WorldResource->Debug_CommandDrawIndexed_Line(cmdList, FrameContext.MaterialResource, DebugDrawLineMaterialId, DebugDrawLineNoDepthMaterialId, DebugDrawCameraId);
#endif // !RPG_BUILD_SHIPPING

}
