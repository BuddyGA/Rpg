#include "RpgRenderer.h"
#include "RpgRenderResource.h"
#include "RpgMaterial.h"
#include "RpgTexture2D.h"
#include "async_task/RpgAsyncTask_Copy.h"
#include "async_task/RpgAsyncTask_Compute.h"
#include "async_task/RpgAsyncTask_RenderPass.h"



RpgRenderer::RpgRenderer(HWND in_WindowHandle, bool bEnableVsync) noexcept
	: FrameDatas()
{
	WindowHandle = in_WindowHandle;

	IDXGIFactory6* factory = RpgD3D12::GetFactory();
	BOOL bCheckTearingSupport = FALSE;

	if (SUCCEEDED(factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bCheckTearingSupport, sizeof(BOOL))))
	{
		bSupportTearing = 1;
	}

	bSupportHDR = 0;
	bPendingChangeVsync = bEnableVsync;
	bVsync = 0;

	BackbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	for (int f = 0; f < RPG_RENDER_FRAME_BUFFERING; ++f)
	{
		FFrameData& frame = FrameDatas[f];
		RPG_D3D12_Validate(RpgD3D12::GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frame.Fence)));
		frame.FenceValue = 0;
		frame.AsyncTaskCopy = RpgThreadPool::CreateTask<RpgAsyncTask_Copy>();
		frame.MaterialResource = new RpgMaterialResource();
		frame.MeshResource = new RpgMeshResource();
		frame.MeshSkinnedResource = new RpgMeshSkinnedResource();
		frame.AsyncTaskCompute = RpgThreadPool::CreateTask<RpgAsyncTask_Compute>();
		RPG_D3D12_Validate(RpgD3D12::GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame.SwapChainCmdAlloc)));
		RPG_D3D12_Validate(RpgD3D12::GetDevice()->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&frame.SwapChainCmdList)));

	}

	AntiAliasingMode = EAntiAliasingMode::NONE;
	Gamma = 2.2f;
}


RpgRenderer::~RpgRenderer() noexcept
{
	SwapchainWaitAllPresents();
	SwapchainReleaseResources(false);

	for (int f = 0; f < RPG_RENDER_FRAME_BUFFERING; ++f)
	{
		WaitFrameFinished(f);

		FFrameData& frame = FrameDatas[f];
		frame.Fence.Reset();
		
		RpgThreadPool::DestroyTask(frame.AsyncTaskCopy);

		delete frame.MaterialResource;
		delete frame.MeshResource;
		delete frame.MeshSkinnedResource;

		for (int w = 0; w < frame.WorldContexts.GetCount(); ++w)
		{
			FWorldContext& context = frame.WorldContexts[w];
			delete context.Resource;
		}

		RpgThreadPool::DestroyTask(frame.AsyncTaskCompute);

		frame.SwapChainCmdAlloc.Reset();
		frame.SwapChainCmdList.Reset();
	}
}


void RpgRenderer::SwapchainWaitAllPresents() noexcept
{
	HANDLE waitCompletedEvents[RPG_RENDER_FRAME_BUFFERING];
	int waitCount = 0;

	for (int f = 0; f < RPG_RENDER_FRAME_BUFFERING; ++f)
	{
		FFrameData& frame = FrameDatas[f];
		
		if (frame.Fence->GetCompletedValue() < frame.FenceValue)
		{
			frame.Fence->SetEventOnCompletion(frame.FenceValue, frame.PresentCompletedEvent);
			waitCompletedEvents[waitCount++] = frame.PresentCompletedEvent;
		}
	}

	if (waitCount > 0)
	{
		WaitForMultipleObjects(waitCount, waitCompletedEvents, TRUE, INFINITE);
	}
}


void RpgRenderer::SwapchainReleaseResources(bool bResize) noexcept
{
	for (int f = 0; f < RPG_RENDER_FRAME_BUFFERING; ++f)
	{
		BackbufferResources[f].Reset();
	}

	if (!bResize)
	{
		SwapChain.Reset();
	}
}


void RpgRenderer::SwapchainResize() noexcept
{
	RECT windowRect;
	GetClientRect(WindowHandle, &windowRect);
	RpgPointInt windowDimension(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);

	// Maybe minimized
	if (windowDimension.X == 0 && windowDimension.Y == 0)
	{
		return;
	}

	DXGI_SWAP_CHAIN_DESC1 desc{};

	if (SwapChain)
	{
		SwapChain->GetDesc1(&desc);
	}

	bool bShouldResize = (SwapChain == nullptr);

	if (static_cast<UINT>(windowDimension.X) != desc.Width || static_cast<UINT>(windowDimension.Y) != desc.Height)
	{
		RPG_PLATFORM_LogDebug(RpgLogD3D12, "Resizing swapchain... Adjusting to window size (Swapchain: %i, %i) (Window: %i, %i)!", desc.Width, desc.Height, windowDimension.X, windowDimension.Y);
		bShouldResize = true;
	}

	if (bVsync != bPendingChangeVsync)
	{
		RPG_PLATFORM_LogDebug(RpgLogD3D12, "Resizing swapchain... Vsync changed!");
		bShouldResize = true;
	}

	if (!bShouldResize)
	{
		return;
	}

	bVsync = bPendingChangeVsync;
	const UINT flags = bSupportTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	if (SwapChain == nullptr)
	{
		RPG_PLATFORM_LogDebug(RpgLogD3D12, "Create swapchain");

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
		swapchainDesc.Flags = flags;
		swapchainDesc.BufferCount = RPG_RENDER_FRAME_BUFFERING;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.Format = BackbufferFormat;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc{};
		fullscreenDesc.Windowed = TRUE;

		IDXGIFactory6* factory = RpgD3D12::GetFactory();

		ComPtr<IDXGISwapChain1> tempSwapchain;
		RPG_D3D12_Validate(factory->CreateSwapChainForHwnd(RpgD3D12::GetCommandQueueDirect(), WindowHandle, &swapchainDesc, &fullscreenDesc, nullptr, &tempSwapchain));
		RPG_D3D12_Validate(factory->MakeWindowAssociation(WindowHandle, DXGI_MWA_NO_ALT_ENTER));
		RPG_D3D12_Validate(tempSwapchain->QueryInterface(IID_PPV_ARGS(&SwapChain)));
	}
	else
	{
		SwapchainWaitAllPresents();
		SwapchainReleaseResources(true);

		RPG_PLATFORM_LogDebug(RpgLogD3D12, "Resize swapchain %i, %i", windowDimension.X, windowDimension.Y);
		RPG_D3D12_Validate(SwapChain->ResizeBuffers(RPG_RENDER_FRAME_BUFFERING, static_cast<UINT>(windowDimension.X), static_cast<UINT>(windowDimension.Y), BackbufferFormat, flags));
	}

	for (int f = 0; f < RPG_RENDER_FRAME_BUFFERING; ++f)
	{
		RPG_D3D12_Validate(SwapChain->GetBuffer(f, IID_PPV_ARGS(&BackbufferResources[f])));
		RPG_D3D12_SetDebugName(BackbufferResources[f], "_%i_RES_SwapBackbuffer", f);
	}

	RPG_PLATFORM_LogDebug(RpgLogD3D12, "Swapchain resized successfully");

	BackbufferIndex = SwapChain->GetCurrentBackBufferIndex();
}


void RpgRenderer::WaitFrameFinished(int frameIndex) noexcept
{
	FFrameData& frame = FrameDatas[frameIndex];

	const uint64_t fenceCompletedValue = frame.Fence->GetCompletedValue();

	if (fenceCompletedValue < frame.FenceValue)
	{
		HANDLE waitFenceCompletedHandle = CreateEventA(NULL, FALSE, FALSE, NULL);
		frame.Fence->SetEventOnCompletion(frame.FenceValue, waitFenceCompletedHandle);
		WaitForSingleObject(waitFenceCompletedHandle, INFINITE);
		CloseHandle(waitFenceCompletedHandle);
	}
}


void RpgRenderer::RegisterWorld(const RpgWorld* world) noexcept
{
	RPG_PLATFORM_Assert(world);

	for (int f = 0; f < RPG_RENDER_FRAME_BUFFERING; ++f)
	{
		FFrameData& frame = FrameDatas[f];

		if (frame.WorldContexts.FindIndexByCompare(world) == RPG_INDEX_INVALID)
		{
			FWorldContext& context = frame.WorldContexts.Add();
			context.World = world;
			context.Resource = new RpgWorldResource();
		}
	}
}


void RpgRenderer::UnregisterWorld(const RpgWorld* world) noexcept
{
	RPG_PLATFORM_Assert(world);

	RPG_NotImplementedYet();
}


void RpgRenderer::BeginRender(int frameIndex, float deltaTime) noexcept
{
	WaitFrameFinished(frameIndex);

	FFrameData& frame = FrameDatas[frameIndex];
	frame.MaterialResource->Reset();
	frame.MeshResource->Reset();
	frame.MeshSkinnedResource->Reset();

	for (int i = 0; i < frame.WorldContexts.GetCount(); ++i)
	{
		FWorldContext& context = frame.WorldContexts[i];
		context.Resource->Reset();
		context.Resource->SetDeltaTime(deltaTime);
		context.Viewports.Clear();
	}

	SwapchainResize();

	Renderer2d.Begin(frameIndex, GetSwapChainDimension());

	FinalTexture.Release();
}


void RpgRenderer::EndRender(int frameIndex) noexcept
{
	Renderer2d.End(frameIndex);

	FFrameData& frame = FrameDatas[frameIndex];

	RpgMeshResource* meshResource = frame.MeshResource;
	RpgMeshSkinnedResource* meshSkinnedResource = frame.MeshSkinnedResource;
	RpgMaterialResource* materialResource = frame.MaterialResource;

	// pre-render
	FWorldContextArray& worldContexts = frame.WorldContexts;

	for (int w = 0; w < worldContexts.GetCount(); ++w)
	{
		FWorldContext& context = worldContexts[w];

		for (int v = 0; v < context.Viewports.GetCount(); ++v)
		{
			context.Viewports[v]->PreRender(frameIndex, materialResource, meshResource, meshSkinnedResource, context.Resource, context.World);
		}
	}

	// Renderer2d 
	Renderer2d.PreRender(frameIndex, materialResource);

	// Default material fullscreen
	RpgMaterialResource::FMaterialID fullscreenMaterialResourceId;
	{
		RpgSharedMaterial defMatFullscreen = RpgMaterial::s_GetDefault(RpgMaterialDefault::FULLSCREEN);
		if (FinalTexture)
		{
			defMatFullscreen->SetParameterTextureValue(RpgMaterialParameterTexture::BASE_COLOR, FinalTexture);
		}

		defMatFullscreen->SetParameterScalarValue("gamma", Gamma);
		fullscreenMaterialResourceId = materialResource->AddMaterial(defMatFullscreen);
	}


	// update resource
	{
		materialResource->UpdateResources();
		meshResource->UpdateResources();
		meshSkinnedResource->UpdateResources();

		for (int w = 0; w < worldContexts.GetCount(); ++w)
		{
			worldContexts[w].Resource->UpdateResources();
		}
	}

	// async copy
	RpgAsyncTask_Copy* asyncCopy = frame.AsyncTaskCopy;
	{
		asyncCopy->Reset();
		asyncCopy->FrameIndex = frameIndex;
		asyncCopy->FenceSignal = frame.Fence.Get();
		asyncCopy->FenceSignalValue = ++frame.FenceValue;
		asyncCopy->Renderer2d = &Renderer2d;
		asyncCopy->MaterialResource = frame.MaterialResource;
		asyncCopy->MeshResource = frame.MeshResource;
		asyncCopy->MeshSkinnedResource = frame.MeshSkinnedResource;

		for (int w = 0; w < worldContexts.GetCount(); ++w)
		{
			asyncCopy->WorldResources.AddValue(worldContexts[w].Resource);
		}

	#if RPG_RENDER_MULTITHREADED
		RpgThreadPool::SubmitTasks((RpgThreadTask**)&asyncCopy, 1);
	#else
		asyncCopy->Execute();
	#endif // RPG_RENDER_MULTITHREADED
	}


	// async compute
	RpgAsyncTask_Compute* asyncCompute = frame.AsyncTaskCompute;
	{
		asyncCompute->Reset();
		asyncCompute->FenceSignal = frame.Fence.Get();
		asyncCompute->WaitFenceCopyValue = frame.FenceValue;
		asyncCompute->FenceSignalValue = ++frame.FenceValue;
		asyncCompute->MeshResource = frame.MeshResource;
		asyncCompute->MeshSkinnedResource = frame.MeshSkinnedResource;

	#if RPG_RENDER_MULTITHREADED
		RpgThreadPool::SubmitTasks((RpgThreadTask**)&asyncCompute, 1);
	#else
		asyncCompute->Execute();
	#endif // RPG_RENDER_MULTITHREADED
	}


	// async render pass
	RpgAsyncTask_RenderPassArray asyncRenderPasses;

	for (int w = 0; w < worldContexts.GetCount(); ++w)
	{
		FWorldContext& context = worldContexts[w];

		for (int v = 0; v < context.Viewports.GetCount(); ++v)
		{
			context.Viewports[v]->SetupRenderPasses(frameIndex, asyncRenderPasses, materialResource, meshResource, meshSkinnedResource, context.Resource);
		}
	}

#if RPG_RENDER_MULTITHREADED
	if (!asyncRenderPasses.IsEmpty())
	{
		RpgThreadPool::SubmitTasks((RpgThreadTask**)asyncRenderPasses.GetData(), asyncRenderPasses.GetCount());
	}
#else
	for (int r = 0; r < asyncRenderPasses.GetCount(); ++r)
	{
		asyncRenderPasses[r]->Execute();
	}
#endif // RPG_RENDER_MULTITHREADED

	
	// Begin swapchain rendering
	const RpgPointInt swapchainDimension = GetSwapChainDimension();
	ID3D12Resource* swapchainBackbuffer = BackbufferResources[BackbufferIndex].Get();
	RpgD3D12::FResourceDescriptor backbufferDescriptor = RpgD3D12::AllocateDescriptor_RTV(swapchainBackbuffer);

	ID3D12GraphicsCommandList* cmdList = frame.SwapChainCmdList.Get();
	RPG_D3D12_COMMAND_Begin(frame.SwapChainCmdAlloc, cmdList);
	{
		// Set viewport
		RpgD3D12Command::SetViewport(cmdList, 0, 0, swapchainDimension.X, swapchainDimension.Y, 0.0f, 1.0f);

		// Set scissor
		RpgD3D12Command::SetScissor(cmdList, 0, 0, swapchainDimension.X, swapchainDimension.Y);

		// Transition backbuffer resource to render target
		RpgD3D12Command::TransitionAllSubresources(cmdList, swapchainBackbuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// Set and clear render target
		RpgD3D12Command::SetAndClearRenderTargets(cmdList, &backbufferDescriptor, 1, RpgColorLinear(0.1f, 0.15f, 0.2f), nullptr, 1.0f, 0);

		// Bind shader resource material
		materialResource->CommandBindShaderResources(cmdList);

		// Set topology triangle-list
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Draw fullscreen final texture
		if (FinalTexture)
		{
			if (FinalTexture->IsRenderTarget() || FinalTexture->IsDepthStencil())
			{
				ID3D12Resource* textureResource = FinalTexture->GPU_GetResource();

				RpgD3D12Command::TransitionAllSubresources(cmdList, textureResource,
					FinalTexture->IsRenderTarget() ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_DEPTH_WRITE,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
				);
			}

			materialResource->CommandBindMaterial(cmdList, fullscreenMaterialResourceId);

			cmdList->DrawInstanced(3, 1, 0, 0);
		}

		// Render 2D
		Renderer2d.CommandDraw(frameIndex, cmdList, materialResource);

		// Transition backbuffer resource to present
		RpgD3D12Command::TransitionAllSubresources(cmdList, swapchainBackbuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}
	RPG_D3D12_COMMAND_End(cmdList);


	RpgArrayInline<ID3D12CommandList*, 8> directCommandLists;

	// Wait all render passes
	for (int i = 0; i < asyncRenderPasses.GetCount(); ++i)
	{
		RpgAsyncTask_RenderPass* renderPass = asyncRenderPasses[i];

	#if RPG_RENDER_MULTITHREADED
		renderPass->Wait();
	#endif // RPG_RENDER_MULTITHREADED

		directCommandLists.AddValue(renderPass->GetCommandList());
	}

	directCommandLists.AddValue(cmdList);

	// Execute/submit recorded direct command lists
	ID3D12CommandQueue* cmdQueueDirect = RpgD3D12::GetCommandQueueDirect();
	if (!directCommandLists.IsEmpty())
	{
		RPG_D3D12_Validate(cmdQueueDirect->Wait(frame.Fence.Get(), frame.FenceValue));
		cmdQueueDirect->ExecuteCommandLists(directCommandLists.GetCount(), directCommandLists.GetData());

		// Present
		if (frame.PresentCompletedEvent == NULL)
		{
			frame.PresentCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		}

		HRESULT hr = SwapChain->Present(bVsync, (bSupportTearing && !bVsync) ? DXGI_PRESENT_ALLOW_TEARING : 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			RPG_PLATFORM_LogError(RpgLogD3D12, "Present failed: DXGI_ERROR_DEVICE_REMOVED!");
		}
		else if (hr == DXGI_ERROR_DEVICE_RESET)
		{
			RPG_PLATFORM_LogError(RpgLogD3D12, "Present failed: DXGI_ERROR_DEVICE_RESET!");
		}
		else if (hr != S_OK)
		{
			RPG_PLATFORM_LogWarn(RpgLogD3D12, "Present failed!");
		}

		BackbufferIndex = SwapChain->GetCurrentBackBufferIndex();

		RPG_D3D12_Validate(cmdQueueDirect->Signal(frame.Fence.Get(), ++frame.FenceValue));
	}


	if (FinalTexture && (FinalTexture->IsRenderTarget() || FinalTexture->IsDepthStencil()))
	{
		FinalTexture->GPU_SetState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}


#if RPG_RENDER_MULTITHREADED
	asyncCopy->Wait();
	asyncCompute->Wait();
#endif // RPG_RENDER_MULTITHREADED

}




#ifndef RPG_BUILD_SHIPPING

RpgVertexPrimitiveBatchLine* RpgRenderer::Debug_GetPrimitiveBatchLine(int frameIndex, const RpgWorld* world, bool bNoDepth) noexcept
{
	RpgWorldResource* resource = GetWorldContext(frameIndex, world).Resource;
	RPG_PLATFORM_Assert(resource);

	return bNoDepth ? &resource->DebugLineNoDepth : &resource->DebugLine;
}

#endif // !RPG_BUILD_SHIPPING
