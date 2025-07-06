#include "RpgGameApp.h"
#include "RpgGameState.h"
#include "core/RpgCommandLine.h"
#include "render/RpgRenderer.h"
#include "render/RpgSceneViewport.h"
#include "collision/RpgCollisionComponent.h"
#include "collision/RpgCollisionWorldSubsystem.h"
#include "render/RpgRenderComponent.h"
#include "render/RpgRenderWorldSubsystem.h"
#include "animation/RpgAnimationComponent.h"
#include "animation/RpgAnimationWorldSubsystem.h"
#include "gui/RpgGuiContext.h"



RPG_PLATFORM_LOG_DEFINE_CATEGORY(RpgLogGame, VERBOSITY_DEBUG)


RpgGameApp* g_GameApp = nullptr;


RpgGameApp::RpgGameApp(const char* windowTitle) noexcept
{
	RpgPointInt windowResolution(1600, 900);

	const int cmdArgResX = RpgCommandLine::GetCommandValueInt("resx");
	if (cmdArgResX > 0)
	{
		windowResolution.X = cmdArgResX;
	}

	const int cmdArgResY = RpgCommandLine::GetCommandValueInt("resy");
	if (cmdArgResY > 0)
	{
		windowResolution.Y = cmdArgResY;
	}

	RPG_PLATFORM_Log(RpgLogSystem, "Create game window (%i, %i)", windowResolution.X, windowResolution.Y);

	// main window
	Window = SDL_CreateWindow(windowTitle, windowResolution.X, windowResolution.Y, SDL_WINDOW_RESIZABLE);
	NativeWindowHandle = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
	RpgPlatformProcess::SetMainWindowHandle(NativeWindowHandle);


	// main world
	{
		MainWorld = new RpgWorld("world_main");

		// Subsystems
		MainWorld->Subsystem_Add<RpgCollisionWorldSubsystem>(0);
		MainWorld->Subsystem_Add<RpgAnimationWorldSubsystem>(1);
		MainWorld->Subsystem_Add<RpgRenderWorldSubsystem>(2);

		// Components
		MainWorld->Component_AddStorage<RpgCollisionComponent_Primitive>();
		MainWorld->Component_AddStorage<RpgRenderComponent_Camera>();
		MainWorld->Component_AddStorage<RpgRenderComponent_Mesh>();
		MainWorld->Component_AddStorage<RpgRenderComponent_Light>();
		MainWorld->Component_AddStorage<RpgAnimationComponent>();
	}


	// main renderer
	Renderer = new RpgRenderer(NativeWindowHandle, !RpgCommandLine::HasCommand("novsync"));

	// main viewport
	Viewport = new RpgSceneViewport();

	// gui context
	GuiContext = new RpgGuiContext();

	// game state
	GameStateMachine = new RpgGameStateMachine();

	// cameras
	const RpgRectInt cameraViewportRect(0, 0, windowResolution.X, windowResolution.Y);

	CameraFreeFly.SetRotationPitchYaw(70.0f, 0.0f);
	CameraFreeFly.SetPosition(RpgVector3(0.0f, 300.0f, -200.0f));
	CameraFreeFly.SetViewportRect(cameraViewportRect);

	CameraTopDown.SetRotationPitchYaw(70.0f, 0.0f);
	CameraTopDown.SetPosition(RpgVector3(0.0f, 300.0f, -200.0f));
	CameraTopDown.SetViewportRect(cameraViewportRect);

	CameraActive = &CameraFreeFly;
	CameraActive->Activated();

	FpsLimit = 60;
	FpsSampleTimer = 0.0f;
	FpsSampleFrameCount = 0;
	FpsTimeMs = 0.0f;
	FpsCountMs = 0;
}


RpgGameApp::~RpgGameApp() noexcept
{
	delete GameStateMachine;
	delete GuiContext;
	delete Viewport;
	delete Renderer;
	delete MainWorld;

	SDL_DestroyWindow(Window);
}


void RpgGameApp::Initialize() noexcept
{
	CreateTestLevel();

	GameStateMachine->SetState("EDITOR");
}


void RpgGameApp::WindowSizeChanged(const SDL_WindowEvent& e) noexcept
{
	const RpgRectInt cameraViewportRect(0, 0, e.data1, e.data2);
	CameraFreeFly.SetViewportRect(cameraViewportRect);
	CameraTopDown.SetViewportRect(cameraViewportRect);
}


void RpgGameApp::MouseMove(const SDL_MouseMotionEvent& e) noexcept
{
	GameStateMachine->GetCurrentState<RpgGameState>()->MouseMove(e);
	CameraActive->MouseMove(e);
}


void RpgGameApp::MouseButton(const SDL_MouseButtonEvent& e) noexcept
{
	GameStateMachine->GetCurrentState<RpgGameState>()->MouseButton(e);
	CameraActive->MouseButton(e);
}


void RpgGameApp::KeyboardButton(const SDL_KeyboardEvent& e) noexcept
{
	GameStateMachine->GetCurrentState<RpgGameState>()->KeyboardButton(e);

	CameraActive->KeyboardButton(e);

	if (e.down)
	{
		if (e.scancode == SDL_SCANCODE_EQUALS)
		{
			Renderer->Gamma += 0.01f;
		}
		else if (e.scancode == SDL_SCANCODE_MINUS)
		{
			Renderer->Gamma -= 0.01f;
		}
		else if (e.scancode == SDL_SCANCODE_0)
		{
			Viewport->bFrustumCulling = !Viewport->bFrustumCulling;
		}
		else if (e.scancode == SDL_SCANCODE_9)
		{
			RpgAnimationWorldSubsystem* subsystem = MainWorld->Subsystem_Get<RpgAnimationWorldSubsystem>();
			subsystem->bDebugDrawSkeletonBones = !subsystem->bDebugDrawSkeletonBones;
		}
		else if (e.scancode == SDL_SCANCODE_8)
		{
			RpgRenderWorldSubsystem* subsystem = MainWorld->Subsystem_Get<RpgRenderWorldSubsystem>();
			subsystem->bDebugDrawMeshBounds = !subsystem->bDebugDrawMeshBounds;
		}
		else if (e.scancode == SDL_SCANCODE_F9)
		{
			if (MainWorld->HasStartedPlay())
			{
				MainWorld->DispatchStopPlay();
			}
			else
			{
				MainWorld->DispatchStartPlay();
			}
		}
	}
}


void RpgGameApp::FrameTick(int frameIndex, float deltaTime) noexcept
{
	RpgPointInt windowDimension;
	SDL_GetWindowSize(Window, &windowDimension.X, &windowDimension.Y);


	// Calculate average FPS
	{
		const int FPS_SAMPLE_COUNT = 12;

		if (FpsSampleFrameCount == FPS_SAMPLE_COUNT)
		{
			FpsTimeMs = (FpsSampleTimer * 1000.0f) / FPS_SAMPLE_COUNT;
			FpsCountMs = static_cast<int>(FPS_SAMPLE_COUNT / FpsSampleTimer);
			FpsSampleTimer = 0.0f;
			FpsSampleFrameCount = 0;
			FpsString = RpgString::Format("%.2f ms (%i FPS)", FpsTimeMs, FpsCountMs);
		}

		FpsSampleTimer += deltaTime;
		++FpsSampleFrameCount;
	}


	// GUI
	{
		GuiContext->Begin(RpgRectInt(0, 0, windowDimension.X, windowDimension.Y));

		//GuiContext->AddRect(RpgPointInt(128, 128), RpgColorRGBA(255, 0, 0));
		//GuiContext->AddRect(RpgPointInt(128, 128), RpgColorRGBA(0, 255, 0));
		//GuiContext->AddRect(RpgPointInt(128, 128), RpgColorRGBA(0, 0, 255));
	}


	// Tick update
	{
		GameStateMachine->TickUpdate(deltaTime);
		MainWorld->DispatchTickUpdate(deltaTime);
		CameraActive->TickUpdate(deltaTime);
	}


	// Post tick update
	{
		MainWorld->DispatchPostTickUpdate();
	}


	// Render
	Renderer->BeginRender(frameIndex, deltaTime);
	{
		// Setup main scene viewport
		Viewport->SetCameraRotationAndPosition(CameraActive->GetRotationQuaternion(), CameraActive->GetPosition());
		Viewport->RenderTargetDimension = windowDimension;

		Renderer->RegisterWorld(MainWorld);
		Renderer->AddWorldViewport(frameIndex, MainWorld, Viewport);
		Renderer->FinalTexture = Viewport->GetRenderTargetTexture(frameIndex);

		// Dispatch render
		MainWorld->DispatchRender(frameIndex, Renderer);

		// Render 2D
		RpgRenderer2D& r2 = Renderer->GetRenderer2D();

		// GUI
		GuiContext->End(r2);

		// Debug info
		{
			static RpgString debugInfoText;

			const RpgVector3 position = CameraActive->GetPosition();
			float pitch, yaw;
			CameraActive->GetRotationPitchYaw(pitch, yaw);

			debugInfoText = RpgString::Format(
				"GameState: %s\n"
				"CameraMode: %s\n"
				"CameraPosition: %.2f, %.2f, %.2f\n"
				"CameraPitchYaw: %.2f, %.2f\n"
				"CameraFrustumCulling: %d\n"
				"Gamma: %.2f\n"
				"VSync: %d\n"
				"\n"
				"GameObject: %i\n"
				, *GameStateMachine->GetCurrentState<RpgGameState>()->GetName()
				, CameraActive == &CameraFreeFly ? "FREE_FLY" : "TOP_DOWN"
				, position.X, position.Y, position.Z
				, pitch, yaw
				, Viewport->bFrustumCulling
				, Renderer->Gamma
				, Renderer->GetVsync()
				, MainWorld->GameObject_GetCount()
			);

			r2.AddText(*debugInfoText, debugInfoText.GetLength(), 8, 16, RpgColorRGBA(255, 255, 255));
		}

		// Fps info
		{
			RpgColorRGBA fpsTextColor;

			if (FpsCountMs < 30)
			{
				fpsTextColor = RpgColorRGBA::RED;
			}
			else if (FpsCountMs < 50)
			{
				fpsTextColor = RpgColorRGBA::YELLOW;
			}
			else
			{
				fpsTextColor = RpgColorRGBA::GREEN;
			}

			r2.AddText(*FpsString, FpsString.GetLength(), r2.GetViewportDimension().X - 110, 16, fpsTextColor);
		}
	}
	Renderer->EndRender(frameIndex);


	// Post render
	MainWorld->PostRender();
}
