#include "RpgEngine.h"
#include "core/RpgCommandLine.h"
#include "physics/world/RpgPhysicsComponent.h"
#include "physics/world/RpgPhysicsWorldSubsystem.h"
#include "render/world/RpgRenderComponent.h"
#include "render/world/RpgRenderWorldSubsystem.h"
#include "animation/world/RpgAnimationComponent.h"
#include "animation/world/RpgAnimationWorldSubsystem.h"




RPG_LOG_DEFINE_CATEGORY(RpgLogEngine, VERBOSITY_DEBUG)


RpgEngine* g_Engine = nullptr;


RpgEngine::RpgEngine(const char* windowTitle) noexcept
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

	RPG_Log(RpgLogEngine, "Create game window (%i, %i)", windowResolution.X, windowResolution.Y);

	// main window
	Window = SDL_CreateWindow(windowTitle, windowResolution.X, windowResolution.Y, SDL_WINDOW_RESIZABLE);
	NativeWindowHandle = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
	RpgPlatformProcess::SetMainWindowHandle(NativeWindowHandle);


	// main world
	{
		MainWorld = CreateWorld("world_main");

		// Subsystems
		MainWorld->Subsystem_Add<RpgPhysicsWorldSubsystem>(0);
		MainWorld->Subsystem_Add<RpgAnimationWorldSubsystem>(1);
		MainWorld->Subsystem_Add<RpgRenderWorldSubsystem>(2);

		// Components
		MainWorld->Component_Register<RpgPhysicsComponent_Collision>();
		MainWorld->Component_Register<RpgRenderComponent_Camera>();
		MainWorld->Component_Register<RpgRenderComponent_Mesh>();
		MainWorld->Component_Register<RpgRenderComponent_Light>();
		MainWorld->Component_Register<RpgAnimationComponent>();
	}


	// main renderer
	Renderer = RpgPointer::MakeUnique<RpgRenderer>(NativeWindowHandle, !RpgCommandLine::HasCommand("novsync"));

	// fps info
	FpsLimit = 60;
	FpsSampleTimer = 0.0f;
	FpsSampleFrameCount = 0;
	FpsTimeMs = 0.0f;
	FpsCountMs = 0;
}


RpgEngine::~RpgEngine() noexcept
{
	SDL_DestroyWindow(Window);
}


void RpgEngine::Initialize() noexcept
{
	CreateTestLevel();

	SetMainCamera(MainWorld->GameObject_Create("camera_main"));
	MainWorld->GameObject_AttachScript(MainCameraObject, &ScriptDebugCamera);
}


void RpgEngine::WindowSizeChanged(const SDL_WindowEvent& e) noexcept
{
	const RpgRectInt cameraViewportRect(0, 0, e.data1, e.data2);
}


void RpgEngine::MouseMove(const SDL_MouseMotionEvent& e) noexcept
{
	InputManager.MouseMove(e);
}


void RpgEngine::MouseButton(const SDL_MouseButtonEvent& e) noexcept
{
	InputManager.MouseButton(e);
}


void RpgEngine::KeyboardButton(const SDL_KeyboardEvent& e) noexcept
{
	InputManager.KeyboardButton(e);

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
			RpgRenderComponent_Camera* cameraComp = MainWorld->GameObject_GetComponent<RpgRenderComponent_Camera>(MainCameraObject);
			cameraComp->bFrustumCulling = !cameraComp->bFrustumCulling;
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


void RpgEngine::FrameTick(int frameIndex, float deltaTime) noexcept
{
	RpgPointInt windowDimension;
	SDL_GetWindowSize(Window, &windowDimension.X, &windowDimension.Y);


	// Calculate average FPS
	{
		const int FPS_SAMPLE_COUNT = 3;

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


	// Begin frame
	{
		MainWorld->BeginFrame(frameIndex);

		RpgRenderComponent_Camera* cameraComp = MainWorld->GameObject_GetComponent<RpgRenderComponent_Camera>(MainCameraObject);
		cameraComp->RenderTargetDimension = windowDimension;
	}


	// GUI
	{
		GuiContext.Begin(RpgRectInt(0, 0, windowDimension.X, windowDimension.Y));

		//GuiContext->AddRect(RpgPointInt(128, 128), RpgColorRGBA(255, 0, 0));
		//GuiContext->AddRect(RpgPointInt(128, 128), RpgColorRGBA(0, 255, 0));
		//GuiContext->AddRect(RpgPointInt(128, 128), RpgColorRGBA(0, 0, 255));
	}


	// Tick update
	{
		MainWorld->DispatchTickUpdate(deltaTime);
	}


	// Post tick update
	{
		MainWorld->DispatchPostTickUpdate();
	}


	// Render
	Renderer->BeginRender(frameIndex, deltaTime);
	{
		Renderer->RegisterWorld(MainWorld);

		// Setup renderer default final texture
		Renderer->FinalTexture = SceneViewport.GetRenderTargetTexture(frameIndex);

		// Dispatch render
		MainWorld->DispatchRender(frameIndex, Renderer.Get());

		// Render 2D
		RpgRenderer2D& r2 = Renderer->GetRenderer2D();

		// GUI
		GuiContext.End(r2);

		// Debug info
		{
			static RpgString debugInfoText;

			RpgTransform mainCameraTransform = MainWorld->GameObject_GetWorldTransform(MainCameraObject);
			float pitch, yaw;
			ScriptDebugCamera.GetRotationPitchYaw(pitch, yaw);
			
			debugInfoText = RpgString::Format(
				"CameraPosition: %.2f, %.2f, %.2f\n"
				"CameraPitchYaw: %.2f, %.2f\n"
				"CameraFrustumCulling: %d\n"
				"Gamma: %.2f\n"
				"VSync: %d\n"
				"\n"
				"GameObject: %i\n"
				, mainCameraTransform.Position.X, mainCameraTransform.Position.Y, mainCameraTransform.Position.Z
				, pitch, yaw
				, SceneViewport.bFrustumCulling
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


	// End frame
	MainWorld->EndFrame(frameIndex);
	InputManager.Flush();
}


RpgWorld* RpgEngine::CreateWorld(const RpgName& name) noexcept
{
	const int index = Worlds.GetCount();
	Worlds.AddValue(RpgPointer::MakeUnique<RpgWorld>(name));

	return Worlds[index].Get();
}


void RpgEngine::DestroyWorld(RpgWorld*& world) noexcept
{
	RPG_Check(world);

	const int index = Worlds.FindIndexByCompare(world);
	if (index != RPG_INDEX_INVALID)
	{
		Worlds.RemoveAt(index);
		world = nullptr;

		return;
	}

	RPG_LogWarn(RpgLogEngine, "Fail to destroy world. World (%s) not found!", *world->GetName());
}


void RpgEngine::SetMainCamera(RpgGameObjectID cameraObject) noexcept
{
	if (MainCameraObject == cameraObject)
	{
		return;
	}

	MainCameraObject = cameraObject;

	RpgRenderComponent_Camera* cameraComp = MainWorld->GameObject_AddComponent<RpgRenderComponent_Camera>(MainCameraObject);
	cameraComp->Viewport = &SceneViewport;
	cameraComp->bActivated = true;
}
