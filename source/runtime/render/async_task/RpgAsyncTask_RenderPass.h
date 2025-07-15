#pragma once

#include "core/RpgThreadPool.h"
#include "../RpgRenderResource.h"


class RpgTexture2D;



class RpgAsyncTask_RenderPass : public RpgThreadTask
{
private:
	ComPtr<ID3D12CommandAllocator> CmdAllocDirect;
	ComPtr<ID3D12GraphicsCommandList> CmdListDirect;

public:
	RpgRenderFrameContext FrameContext;
	const RpgWorldResource* WorldResource;
	

public:
	RpgAsyncTask_RenderPass() noexcept;
	virtual void Reset() noexcept override;
	virtual void Execute() noexcept override;

protected:
	virtual void CommandDraw(ID3D12GraphicsCommandList* cmdList) const noexcept = 0;

public:
	inline ID3D12CommandList* GetCommandList() const noexcept
	{
		return CmdListDirect.Get();
	}

};



class RpgAsyncTask_RenderPass_Shadow : public RpgAsyncTask_RenderPass
{
public:
	RpgTexture2D* DepthTexture;
	RpgWorldResource::FViewID ViewId;

	const RpgDrawIndexedDepth* DrawMeshData;
	int DrawMeshCount;

	const RpgDrawIndexedDepth* DrawSkinnedMeshData;
	int DrawSkinnedMeshCount;

	bool bIsOmniDirectional;


public:
	RpgAsyncTask_RenderPass_Shadow() noexcept;
	virtual void Reset() noexcept override;

	virtual const char* GetTaskName() const noexcept override
	{
		return "RpgAsyncTask_RenderPass_Shadow";
	}


protected:
	virtual void CommandDraw(ID3D12GraphicsCommandList* cmdList) const noexcept override;

};



class RpgAsyncTask_RenderPass_Forward : public RpgAsyncTask_RenderPass
{
public:
	RpgTexture2D* RenderTargetTexture;
	RpgTexture2D* DepthStencilTexture;

	const RpgDrawIndexed* DrawMeshData;
	int DrawMeshCount;

	const RpgDrawIndexed* DrawSkinnedMeshData;
	int DrawSkinnedMeshCount;


public:
	RpgAsyncTask_RenderPass_Forward() noexcept;
	virtual void Reset() noexcept override;

	virtual const char* GetTaskName() const noexcept override
	{
		return "RpgAsyncRenderPass_Forward";
	}


protected:
	virtual void CommandDraw(ID3D12GraphicsCommandList* cmdList) const noexcept override;


#ifndef RPG_BUILD_SHIPPING
public:
	RpgMaterialResource::FMaterialID DebugDrawLineMaterialId;
	RpgMaterialResource::FMaterialID DebugDrawLineNoDepthMaterialId;
	RpgWorldResource::FViewID DebugDrawCameraId;
#endif // !RPG_BUILD_SHIPPING

};
