#include "RpgAsyncTask_Compute.h"
#include "../RpgRenderPipeline.h"



RpgAsyncTask_Compute::RpgAsyncTask_Compute() noexcept
{
	RPG_D3D12_Validate(RpgD3D12::GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&CmdAllocCompute)));
	RPG_D3D12_SetDebugName(CmdAllocCompute, "CmdAllocCompute_AsyncTaskCompute");

	RPG_D3D12_Validate(RpgD3D12::GetDevice()->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&CmdListCompute)));
	RPG_D3D12_SetDebugName(CmdListCompute, "CmdListCompute_AsyncTaskCompute");

	FenceSignal = nullptr;
	WaitFenceCopyValue = 0;
	FenceSignalValue = 0;
	MeshResource = nullptr;
	MeshSkinnedResource = nullptr;
}


void RpgAsyncTask_Compute::Reset() noexcept
{
	RpgThreadTask::Reset();

	FenceSignal = nullptr;
	WaitFenceCopyValue = 0;
	FenceSignalValue = 0;
	MeshResource = nullptr;
	MeshSkinnedResource = nullptr;
}


void RpgAsyncTask_Compute::Execute() noexcept
{
	RPG_Assert(MeshResource);
	RPG_Assert(MeshSkinnedResource);

	ID3D12GraphicsCommandList* cmdList = CmdListCompute.Get();
	RPG_D3D12_COMMAND_Begin(CmdAllocCompute, CmdListCompute);
	{
		// set compute root-sig
		cmdList->SetComputeRootSignature(RpgRenderPipeline::GetRootSignatureCompute());

		// set skinning pso
		cmdList->SetPipelineState(RpgRenderPipeline::GetComputeSkinningPSO());

		// bind SRV (vertex position)
		ID3D12Resource* vertexPositionResource = MeshSkinnedResource->GetResourceVertexPosition();
		cmdList->SetComputeRootShaderResourceView(RpgRenderPipeline::CRPI_VERTEX_POSITION, vertexPositionResource->GetGPUVirtualAddress());

		// bind SRV (vertex normal-tangent)
		ID3D12Resource* vertexNormalTangentResource = MeshSkinnedResource->GetResourceVertexNormalTangent();
		cmdList->SetComputeRootShaderResourceView(RpgRenderPipeline::CRPI_VERTEX_NORMAL_TANGENT, vertexNormalTangentResource->GetGPUVirtualAddress());

		// bind SRV (vertex skin)
		ID3D12Resource* vertexSkinResource = MeshSkinnedResource->GetResourceVertexSkin();
		cmdList->SetComputeRootShaderResourceView(RpgRenderPipeline::CRPI_VERTEX_SKIN, vertexSkinResource->GetGPUVirtualAddress());

		// bind SRV (skeleton bone)
		ID3D12Resource* skeletonBoneResource = MeshSkinnedResource->GetResourceSkeletonBoneSkinning();
		cmdList->SetComputeRootShaderResourceView(RpgRenderPipeline::CRPI_SKELETON_BONE_DATA, skeletonBoneResource->GetGPUVirtualAddress());

		// bind UAV (skinned vertex position)
		ID3D12Resource* skinnedVertexPositionResource = MeshSkinnedResource->GetResourceSkinnedVertexPosition();
		cmdList->SetComputeRootUnorderedAccessView(RpgRenderPipeline::CRPI_SKINNED_VERTEX_POSITION, skinnedVertexPositionResource->GetGPUVirtualAddress());

		// bind UAV (skinned vertex normal-tangent)
		ID3D12Resource* skinnedVertexNormalTangentResource = MeshSkinnedResource->GetResourceSkinnedVertexNormalTangent();
		cmdList->SetComputeRootUnorderedAccessView(RpgRenderPipeline::CRPI_SKINNED_VERTEX_NORMAL_TANGENT, skinnedVertexNormalTangentResource->GetGPUVirtualAddress());

		// dispatch calls
		const RpgArray<RpgShaderConstantSkinnedObjectParameter>& objectParams = MeshSkinnedResource->GetObjectParameters();

		for (int i = 0; i < objectParams.GetCount(); ++i)
		{
			const RpgShaderConstantSkinnedObjectParameter param = objectParams[i];
			RPG_Check(param.SkeletonIndex != RPG_INDEX_INVALID);

			cmdList->SetComputeRoot32BitConstants(RpgRenderPipeline::CRPI_SKINNED_OBJECT_PARAM, sizeof(RpgShaderConstantSkinnedObjectParameter) / sizeof(UINT), &param, 0);
			cmdList->Dispatch((param.VertexCount + 63) / 64, 1, 1);
		}
	}
	RPG_D3D12_COMMAND_End(CmdListCompute);


	// Execute/submit command list
	ID3D12CommandQueue* cmdQueueCompute = RpgD3D12::GetCommandQueueCompute();
	RPG_D3D12_Validate(cmdQueueCompute->Wait(FenceSignal, WaitFenceCopyValue));
	cmdQueueCompute->ExecuteCommandLists(1, (ID3D12CommandList**)&cmdList);
	RPG_D3D12_Validate(cmdQueueCompute->Signal(FenceSignal, FenceSignalValue));
}
