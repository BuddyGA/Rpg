#pragma once

#include "RpgRenderer2D.h"


class RpgWorld;



class RpgRenderer
{
	RPG_NOCOPY(RpgRenderer)

public:
	RpgRenderer(HWND in_WindowHandle, bool bEnableVsync) noexcept;
	~RpgRenderer() noexcept;


private:
	HWND WindowHandle;

	uint8_t bSupportTearing : 1;
	uint8_t bSupportHDR : 1;
	uint8_t bPendingChangeVsync : 1;
	uint8_t bVsync : 1;

	ComPtr<IDXGISwapChain4> SwapChain;

	DXGI_FORMAT BackbufferFormat;
	ComPtr<ID3D12Resource> BackbufferResources[RPG_FRAME_BUFFERING];
	uint32_t BackbufferIndex;


private:
	void SwapchainWaitAllPresents() noexcept;
	void SwapchainReleaseResources(bool bResize) noexcept;
	void SwapchainResize() noexcept;

public:
	inline void SetVsync(bool bEnabled) noexcept
	{
		if (bVsync == bEnabled)
		{
			return;
		}

		bPendingChangeVsync = bEnabled;
	}

	[[nodiscard]] inline bool GetVsync() const noexcept
	{
		return bVsync;
	}

	[[nodiscard]] RpgPointInt GetSwapChainDimension() const noexcept
	{
		DXGI_SWAP_CHAIN_DESC1 desc{};
		SwapChain->GetDesc1(&desc);

		return RpgPointInt(static_cast<int>(desc.Width), static_cast<int>(desc.Height));
	}


private:
	RpgRenderer2D Renderer2d;


	struct FWorldContext
	{
		const RpgWorld* World = nullptr;
		RpgWorldResource* Resource = nullptr;
		RpgArrayInline<RpgRenderViewport*, 8> Viewports;

		inline bool operator==(const RpgWorld* rhs) const noexcept
		{
			return World == rhs;
		}

	};
	typedef RpgArray<FWorldContext, 4> FWorldContextArray;


	struct FFrameData
	{
		ComPtr<ID3D12Fence> Fence;
		uint64_t FenceValue;

		RpgAsyncTask_Copy* AsyncTaskCopy;

		RpgMaterialResource* MaterialResource;
		RpgMeshResource* MeshResource;
		RpgMeshSkinnedResource* MeshSkinnedResource;
		FWorldContextArray WorldContexts;

		RpgAsyncTask_Compute* AsyncTaskCompute;

		ComPtr<ID3D12CommandAllocator> SwapChainCmdAlloc;
		ComPtr<ID3D12GraphicsCommandList> SwapChainCmdList;
		HANDLE PresentCompletedEvent;
	};
	FFrameData FrameDatas[RPG_FRAME_BUFFERING];


public:
	enum class EAntiAliasingMode : uint8_t
	{
		NONE = 0,
		FXAA,
		SMAA
	};

	EAntiAliasingMode AntiAliasingMode;
	RpgSharedTexture2D FinalTexture;
	float Gamma;

	
private:
	void WaitFrameFinished(int frameIndex) noexcept;


	[[nodiscard]] inline FWorldContext& GetWorldContext(int frameIndex, const RpgWorld* world) noexcept
	{
		FFrameData& frame = FrameDatas[frameIndex];

		const int index = frame.WorldContexts.FindIndexByCompare(world);
		RPG_PLATFORM_Check(index != RPG_INDEX_INVALID);

		return frame.WorldContexts[index];
	}

public:
	void RegisterWorld(const RpgWorld* world) noexcept;
	void UnregisterWorld(const RpgWorld* world) noexcept;


	inline void AddWorldViewport(int frameIndex, const RpgWorld* world, RpgRenderViewport* viewport) noexcept
	{
		GetWorldContext(frameIndex, world).Viewports.AddUnique(viewport);
	}


	void BeginRender(int frameIndex, float deltaTime) noexcept;
	void EndRender(int frameIndex) noexcept;


	[[nodiscard]] inline RpgRenderer2D& GetRenderer2D() noexcept
	{
		return Renderer2d;
	}


#ifndef RPG_BUILD_SHIPPING
public:
	[[nodiscard]] RpgVertexPrimitiveBatchLine* Debug_GetPrimitiveBatchLine(int frameIndex, const RpgWorld* world, bool bNoDepth) noexcept;
#endif // !RPG_BUILD_SHIPPING

};
