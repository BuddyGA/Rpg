#pragma once

#include "core/RpgString.h"
#include "core/RpgPointer.h"
#include "core/world/RpgWorld.h"
#include "input/RpgInputManager.h"
#include "render/RpgRenderer.h"
#include "render/RpgSceneViewport.h"
#include "gui/RpgGuiContext.h"
#include "script/RpgScriptDebugCamera.h"



extern class RpgEngine* g_Engine;

class RpgEngine 
{
	RPG_NOCOPYMOVE(RpgEngine)

private:
	// Main window
	SDL_Window* Window;
	HWND NativeWindowHandle;

	// Input manager
	RpgInputManager InputManager;

	// Created worlds. Main world always at index 0
	RpgArray<RpgUniquePtr<RpgWorld>> Worlds;
	RpgWorld* MainWorld;

	// Main renderer
	RpgUniquePtr<RpgRenderer> Renderer;

	// Main scene viewport
	RpgSceneViewport SceneViewport;
	
	// GUI context
	RpgGuiContext GuiContext;

	// Main camera object inside main world
	RpgGameObjectID MainCameraObject;

	// Script camera
	RpgScriptDebugCamera ScriptDebugCamera;


public:
	RpgEngine(const char* windowTitle) noexcept;
	~RpgEngine() noexcept;

	void Initialize() noexcept;

	void WindowSizeChanged(const SDL_WindowEvent& e) noexcept;
	void MouseMove(const SDL_MouseMotionEvent& e) noexcept;
	void MouseButton(const SDL_MouseButtonEvent& e) noexcept;
	void KeyboardButton(const SDL_KeyboardEvent& e) noexcept;

	void FrameTick(int frameIndex, float deltaTime) noexcept;

	[[nodiscard]] RpgWorld* CreateWorld(const RpgName& name) noexcept;
	void DestroyWorld(RpgWorld*& world) noexcept;

	void SetMainCamera(RpgGameObjectID cameraObject) noexcept;


	inline bool IsWindowMinimized() const noexcept
	{
		return SDL_GetWindowFlags(Window) & SDL_WINDOW_MINIMIZED;
	}


	inline void SetMouseRelativeMode(bool bEnable) noexcept
	{
		SDL_SetWindowRelativeMouseMode(Window, bEnable);
	}

	inline void SetMouseCursorPosition(int x, int y) noexcept
	{
		SDL_WarpMouseInWindow(Window, static_cast<float>(x), static_cast<float>(y));
	}


	inline void ClipMouseCursor(bool bClip) noexcept
	{
		if (bClip)
		{
			SDL_Rect rect;
			rect.x = 0;
			rect.y = 0;
			SDL_GetWindowSize(Window, &rect.w, &rect.h);
			rect.w -= 2;
			rect.h -= 2;

			SDL_SetWindowMouseRect(Window, &rect);
		}
		else
		{
			SDL_SetWindowMouseRect(Window, nullptr);
		}
	}


	inline RpgInputManager& GetInputManager() noexcept
	{
		return InputManager;
	}

	inline const RpgInputManager& GetInputManager() const noexcept
	{
		return InputManager;
	}

	inline RpgWorld* GetMainWorld() noexcept
	{
		return MainWorld;
	}

	inline const RpgWorld* GetMainWorld() const noexcept
	{
		return MainWorld;
	}


public:
	int FpsLimit;

private:
	float FpsSampleTimer;
	int FpsSampleFrameCount;
	float FpsTimeMs;
	int FpsCountMs;
	RpgString FpsString;


private:
	void CreateTestLevel() noexcept;

};
