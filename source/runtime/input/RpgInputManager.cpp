#include "RpgInputManager.h"


RPG_LOG_DEFINE_CATEGORY(RpgLogInput, VERBOSITY_DEBUG)



namespace RpgInput
{
	constexpr RpgInputKey::EButton MAP_TO_MOUSE_BUTTON[] =
	{
		RpgInputKey::MOUSE_LEFT,
		RpgInputKey::MOUSE_MIDDLE,
		RpgInputKey::MOUSE_RIGHT,
		RpgInputKey::MOUSE_4,
		RpgInputKey::MOUSE_5,
	};


	constexpr RpgInputKey::EButton MAP_TO_KEYBOARD_BUTTON_0[] =
	{
		RpgInputKey::KEYBOARD_A,
		RpgInputKey::KEYBOARD_B,
		RpgInputKey::KEYBOARD_C,
		RpgInputKey::KEYBOARD_D,
		RpgInputKey::KEYBOARD_E,
		RpgInputKey::KEYBOARD_F,
		RpgInputKey::KEYBOARD_G,
		RpgInputKey::KEYBOARD_H,
		RpgInputKey::KEYBOARD_I,
		RpgInputKey::KEYBOARD_J,
		RpgInputKey::KEYBOARD_K,
		RpgInputKey::KEYBOARD_L,
		RpgInputKey::KEYBOARD_M,
		RpgInputKey::KEYBOARD_N,
		RpgInputKey::KEYBOARD_O,
		RpgInputKey::KEYBOARD_P,
		RpgInputKey::KEYBOARD_Q,
		RpgInputKey::KEYBOARD_R,
		RpgInputKey::KEYBOARD_S,
		RpgInputKey::KEYBOARD_T,
		RpgInputKey::KEYBOARD_U,
		RpgInputKey::KEYBOARD_V,
		RpgInputKey::KEYBOARD_W,
		RpgInputKey::KEYBOARD_X,
		RpgInputKey::KEYBOARD_Y,
		RpgInputKey::KEYBOARD_Z,
		RpgInputKey::KEYBOARD_1,
		RpgInputKey::KEYBOARD_2,
		RpgInputKey::KEYBOARD_3,
		RpgInputKey::KEYBOARD_4,
		RpgInputKey::KEYBOARD_5,
		RpgInputKey::KEYBOARD_6,
		RpgInputKey::KEYBOARD_7,
		RpgInputKey::KEYBOARD_8,
		RpgInputKey::KEYBOARD_9,
		RpgInputKey::KEYBOARD_0,
		RpgInputKey::KEYBOARD_ENTER,
		RpgInputKey::KEYBOARD_ESCAPE,
		RpgInputKey::KEYBOARD_BACKSPACE,
		RpgInputKey::KEYBOARD_TAB,
		RpgInputKey::KEYBOARD_SPACEBAR,
		RpgInputKey::KEYBOARD_MINUS,
		RpgInputKey::KEYBOARD_EQUALS,
		RpgInputKey::KEYBOARD_BRACKET_LEFT,
		RpgInputKey::KEYBOARD_BRACKET_RIGHT,
		RpgInputKey::KEYBOARD_BACKSLASH,
	};


	constexpr RpgInputKey::EButton MAP_TO_KEYBOARD_BUTTON_1[] =
	{
		RpgInputKey::KEYBOARD_SEMICOLON,
		RpgInputKey::KEYBOARD_QUOTE,
		RpgInputKey::KEYBOARD_TILDE,
		RpgInputKey::KEYBOARD_COMMA,
		RpgInputKey::KEYBOARD_PERIOD,
		RpgInputKey::KEYBOARD_SLASH,
		RpgInputKey::KEYBOARD_CAPS_LOCK,
		RpgInputKey::KEYBOARD_F1,
		RpgInputKey::KEYBOARD_F2,
		RpgInputKey::KEYBOARD_F3,
		RpgInputKey::KEYBOARD_F4,
		RpgInputKey::KEYBOARD_F5,
		RpgInputKey::KEYBOARD_F6,
		RpgInputKey::KEYBOARD_F7,
		RpgInputKey::KEYBOARD_F8,
		RpgInputKey::KEYBOARD_F9,
		RpgInputKey::KEYBOARD_F10,
		RpgInputKey::KEYBOARD_F11,
		RpgInputKey::KEYBOARD_F12,
		RpgInputKey::KEYBOARD_PRINTSCREEN,
		RpgInputKey::KEYBOARD_SCROLL_LOCK,
		RpgInputKey::KEYBOARD_PAUSE,
		RpgInputKey::KEYBOARD_INSERT,
		RpgInputKey::KEYBOARD_HOME,
		RpgInputKey::KEYBOARD_PAGE_UP,
		RpgInputKey::KEYBOARD_DELETE,
		RpgInputKey::KEYBOARD_END,
		RpgInputKey::KEYBOARD_PAGE_DOWN,
		RpgInputKey::KEYBOARD_ARROW_RIGHT,
		RpgInputKey::KEYBOARD_ARROW_LEFT,
		RpgInputKey::KEYBOARD_ARROW_DOWN,
		RpgInputKey::KEYBOARD_ARROW_UP,
		RpgInputKey::KEYBOARD_NUM_LOCK,
		RpgInputKey::KEYBOARD_NUMPAD_DIVIDE,
		RpgInputKey::KEYBOARD_NUMPAD_MULTIPLY,
		RpgInputKey::KEYBOARD_NUMPAD_MINUS,
		RpgInputKey::KEYBOARD_NUMPAD_PLUS,
		RpgInputKey::KEYBOARD_NUMPAD_ENTER,
		RpgInputKey::KEYBOARD_NUMPAD_1,
		RpgInputKey::KEYBOARD_NUMPAD_2,
		RpgInputKey::KEYBOARD_NUMPAD_3,
		RpgInputKey::KEYBOARD_NUMPAD_4,
		RpgInputKey::KEYBOARD_NUMPAD_5,
		RpgInputKey::KEYBOARD_NUMPAD_6,
		RpgInputKey::KEYBOARD_NUMPAD_7,
		RpgInputKey::KEYBOARD_NUMPAD_8,
		RpgInputKey::KEYBOARD_NUMPAD_9,
		RpgInputKey::KEYBOARD_NUMPAD_0,
		RpgInputKey::KEYBOARD_NUMPAD_PERIOD,
	};


	constexpr RpgInputKey::EButton MAP_TO_KEYBOARD_BUTTON_2[] =
	{
		RpgInputKey::KEYBOARD_CTRL_LEFT,
		RpgInputKey::KEYBOARD_SHIFT_LEFT,
		RpgInputKey::KEYBOARD_ALT_LEFT,
		RpgInputKey::KEYBOARD_OS_LEFT,
		RpgInputKey::KEYBOARD_CTRL_RIGHT,
		RpgInputKey::KEYBOARD_SHIFT_RIGHT,
		RpgInputKey::KEYBOARD_ATL_RIGHT,
		RpgInputKey::KEYBOARD_OS_RIGHT,
	};

};




RpgInputManager::RpgInputManager() noexcept
{
	RpgPlatformMemory::MemZero(KeyButtonStates[0], sizeof(bool) * RpgInputKey::MAX_COUNT);
	RpgPlatformMemory::MemZero(KeyButtonStates[1], sizeof(bool) * RpgInputKey::MAX_COUNT);
}


void RpgInputManager::MouseMove(const SDL_MouseMotionEvent& e) noexcept
{
	MouseCursorPosition = RpgPointInt(static_cast<int>(e.x), static_cast<int>(e.y));
	MouseCursorDeltaPosition += RpgPointInt(static_cast<int>(e.xrel), static_cast<int>(e.yrel));
}


void RpgInputManager::MouseButton(const SDL_MouseButtonEvent& e) noexcept
{
	const RpgInputKey::EButton mb = RpgInput::MAP_TO_MOUSE_BUTTON[e.button - SDL_BUTTON_LEFT];
	KeyButtonStates[1][mb] = e.down;
}


void RpgInputManager::KeyboardButton(const SDL_KeyboardEvent& e) noexcept
{
	const SDL_Scancode scancode = e.scancode;
	RpgInputKey::EButton kb = RpgInputKey::NONE;

	if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_BACKSLASH)
	{
		kb = RpgInput::MAP_TO_KEYBOARD_BUTTON_0[scancode - SDL_SCANCODE_A];
	}
	else if (scancode >= SDL_SCANCODE_SEMICOLON && scancode <= SDL_SCANCODE_KP_PERIOD)
	{
		kb = RpgInput::MAP_TO_KEYBOARD_BUTTON_1[scancode - SDL_SCANCODE_SEMICOLON];
	}
	else if (scancode >= SDL_SCANCODE_LCTRL && scancode <= SDL_SCANCODE_RGUI)
	{
		kb = RpgInput::MAP_TO_KEYBOARD_BUTTON_2[scancode - SDL_SCANCODE_LCTRL];
	}

	KeyButtonStates[1][kb] = e.down;
}


void RpgInputManager::Flush() noexcept
{
	MouseCursorDeltaPosition = RpgPointInt();

	// save current key button states
	RpgPlatformMemory::MemCopy(KeyButtonStates[0], KeyButtonStates[1], sizeof(bool) * RpgInputKey::MAX_COUNT);
}
