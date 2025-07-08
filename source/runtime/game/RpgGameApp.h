#pragma once

#include "core/RpgString.h"
#include "core/world/RpgGameObject.h"


class RpgInputManager;
class RpgWorld;
class RpgRenderer;
class RpgSceneViewport;
class RpgGuiContext;



class RpgGameApp 
{
	RPG_NOCOPYMOVE(RpgGameApp)

private:
	// Main window
	SDL_Window* Window;
	HWND NativeWindowHandle;

	// Input
	RpgInputManager* InputManager;

	// Main world
	RpgWorld* MainWorld;

	// Main renderer
	RpgRenderer* Renderer;
	
	// Main scene viewport
	RpgSceneViewport* Viewport;

	// GUI context
	RpgGuiContext* GuiContext;

	RpgGameObjectID MainCamera;


public:
	RpgGameApp(const char* windowTitle) noexcept;
	~RpgGameApp() noexcept;

	void Initialize() noexcept;

	void WindowSizeChanged(const SDL_WindowEvent& e) noexcept;
	void MouseMove(const SDL_MouseMotionEvent& e) noexcept;
	void MouseButton(const SDL_MouseButtonEvent& e) noexcept;
	void KeyboardButton(const SDL_KeyboardEvent& e) noexcept;

	void FrameTick(int frameIndex, float deltaTime) noexcept;


	[[nodiscard]] inline bool IsWindowMinimized() const noexcept
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


	[[nodiscard]] inline RpgInputManager* GetInputManager() const noexcept
	{
		return InputManager;
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



extern RpgGameApp* g_GameApp;
