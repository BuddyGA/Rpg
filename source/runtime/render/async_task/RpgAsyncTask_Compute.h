#pragma once

#include "core/RpgThreadPool.h"
#include "../RpgRenderResource.h"



class RpgAsyncTask_Compute : public RpgThreadTask
{
private:
	ComPtr<ID3D12CommandAllocator> CmdAllocCompute;
	ComPtr<ID3D12GraphicsCommandList> CmdListCompute;

public:
	ID3D12Fence* FenceSignal;
	uint64_t WaitFenceCopyValue;
	uint64_t FenceSignalValue;
	const RpgMeshResource* MeshResource;
	const RpgMeshSkinnedResource* MeshSkinnedResource;


public:
	RpgAsyncTask_Compute() noexcept;
	virtual void Reset() noexcept override;
	virtual void Execute() noexcept override;


	virtual const char* GetTaskName() const noexcept override
	{
		return "RpgAsyncTask_Compute";
	}


	[[nodiscard]] inline ID3D12CommandList* GetCommandList() const noexcept
	{
		return CmdListCompute.Get();
	}

};
