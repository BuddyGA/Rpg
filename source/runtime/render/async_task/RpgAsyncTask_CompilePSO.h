#pragma once

#include "core/RpgThreadPool.h"
#include "../RpgMaterial.h"



class RpgAsyncTask_CompilePSO : public RpgThreadTask
{
public:
	ID3D12RootSignature* RootSignature;
	RpgName MaterialName;
	RpgRenderPipelineState PipelineState;

private:
	ComPtr<ID3D12PipelineState> PSO;


public:
	RpgAsyncTask_CompilePSO() noexcept;
	virtual void Reset() noexcept override;
	virtual void Execute() noexcept override;

	virtual const char* GetTaskName() const noexcept override
	{
		return "RpgAsyncTask_CompilePSO";
	}


	[[nodiscard]] inline ComPtr<ID3D12PipelineState> GetCompiledPSO() noexcept
	{
		return std::move(PSO);
	}

};
