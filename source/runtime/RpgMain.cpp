#include "core/RpgCommandLine.h"
#include "core/RpgFilePath.h"
#include "core/RpgThreadPool.h"
#include "core/RpgTimer.h"
#include "render/RpgD3D12.h"
#include "render/RpgTexture2D.h"
#include "render/RpgFont.h"
#include "render/RpgMaterial.h"
#include "asset/RpgAssetImporter.h"
#include "shader/RpgShaderManager.h"
#include "render/RpgRenderPipeline.h"
#include "engine/RpgEngine.h"



int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdShow);


// ------------------------------------------------------------------------------------------------- //
// 	Initialization
// ------------------------------------------------------------------------------------------------- //
	RpgCommandLine::Initialize(lpCmdLine);


#ifdef RPG_BUILD_DEBUG
	RpgPlatformConsole::Initialize();
	RpgPlatformLog::Initialize(RpgPlatformLog::VERBOSITY_DEBUG, "RpgGame.log");

#else
	if (RpgCommandLine::HasCommand("console"))
	{
		RpgPlatformConsole::Initialize();
	}

	RpgPlatformLog::Initialize(RpgPlatformLog::VERBOSITY_LOG, "RpgGame.log");

#endif // RPG_BUILD_DEBUG


	RpgPlatformProcess::Initialize();
	RpgFileSystem::Initialize();

	// TODO: Read config from <RpgGame.config>

	// TODO: Steam init

	RpgThreadPool::Initialize();

	RpgD3D12::Initialize();
	RpgShaderManager::Initialize();
	RpgRenderPipeline::Initialize();

	RpgTexture2D::s_CreateDefaults();
	RpgFont::s_CreateDefaults();
	RpgMaterial::s_CreateDefaults();

	// Compile default materials
	{
		RpgSharedMaterial defaultMaterials[RpgMaterialDefault::MAX_COUNT];
		for (int m = 0; m < RpgMaterialDefault::MAX_COUNT; ++m)
		{
			defaultMaterials[m] = RpgMaterial::s_GetDefault(static_cast<RpgMaterialDefault::EType>(m));
		}

		RpgRenderPipeline::AddMaterials(defaultMaterials, RpgMaterialDefault::MAX_COUNT);
		RpgRenderPipeline::CompileMaterialPSOs(true);
	}


	// Initialize asset manager
	

	// Initialize asset importer
#ifndef RPG_BUILD_SHIPPING
	g_AssetImporter = new RpgAssetImporter();
#endif // !RPG_BUILD_SHIPPING


	// Initialize game
	g_Engine = new RpgEngine("RpgGame_v0.0_alpha");
	g_Engine->Initialize();


// ------------------------------------------------------------------------------------------------- //
// 	Main Loop
// ------------------------------------------------------------------------------------------------- //
	uint64_t FrameCounter = 0;
	bool bRunning = true;

	RpgTimer Timer;
	Timer.Start();


	while (bRunning)
	{
		SDL_Event e{};

		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_EVENT_QUIT:
				{
					bRunning = false;
					break;
				}

				case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
				{
					g_Engine->WindowSizeChanged(e.window);
					break;
				}

				case SDL_EVENT_MOUSE_MOTION:
				{
					g_Engine->MouseMove(e.motion);
					break;
				}

				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				case SDL_EVENT_MOUSE_BUTTON_UP:
				{
					g_Engine->MouseButton(e.button);
					break;
				}

				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP:
				{
					g_Engine->KeyboardButton(e.key);
					break;
				}

				default:
					break;
			}
		}

		if (!bRunning)
		{
			break;
		}


		const int frameIndex = FrameCounter % RPG_FRAME_BUFFERING;
		const uint64_t fpsTickStart = SDL_GetPerformanceCounter();

		RpgD3D12::BeginFrame(frameIndex);

		Timer.Tick();
		g_Engine->FrameTick(frameIndex, Timer.GetDeltaTimeSeconds());

		const int fpsLimit = g_Engine->FpsLimit;

		if (g_Engine->IsWindowMinimized())
		{
			SDL_Delay(60);
		}
		else if (fpsLimit > 0)
		{
			const float fpsTargetTimeMs = 1000.0f / fpsLimit;
			const float fpsDurationMs = static_cast<float>(SDL_GetPerformanceCounter() - fpsTickStart) * 1000.0f / SDL_GetPerformanceFrequency();

			if (fpsDurationMs < fpsTargetTimeMs)
			{
				const uint32_t sleepTimeMs = static_cast<uint32_t>(fpsTargetTimeMs - fpsDurationMs);
				SDL_Delay(sleepTimeMs);
			}
		}

		++FrameCounter;
	}

	RPG_Log(RpgLogSystem, "Exit game");


// ------------------------------------------------------------------------------------------------- //
// 	Shutdown
// ------------------------------------------------------------------------------------------------- //
	delete g_Engine;

	RpgMaterial::s_DestroyDefaults();
	RpgFont::s_DestroyDefaults();
	RpgTexture2D::s_DestroyDefaults();

	RpgShaderManager::Shutdown();
	RpgRenderPipeline::Shutdown();
	RpgD3D12::Shutdown();

	RpgThreadPool::Shutdown();
	RpgPlatformProcess::Shutdown();

	return 0;
}
