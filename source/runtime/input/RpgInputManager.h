#pragma once

#include "RpgInputTypes.h"


class RpgInputManager
{
	RPG_NOCOPYMOVE(RpgInputManager)

private:
	// Mouse cursor position
	RpgPointInt MouseCursorPosition;

	// Mouse cursor delta position
	RpgPointInt MouseCursorDeltaPosition;

	// Mouse wheel value
	RpgPointInt MouseWheelValue;

	// Key button states
	// [0]: Previous, [1]: Current
	bool KeyButtonStates[2][RpgInputKey::MAX_COUNT];


public:
	RpgInputManager() noexcept;

	void MouseMove(const SDL_MouseMotionEvent& e) noexcept;
	void MouseButton(const SDL_MouseButtonEvent& e) noexcept;
	void KeyboardButton(const SDL_KeyboardEvent& e) noexcept;

	void UpdateMappingStates() noexcept;

	// Record current frame key states as previous frame. Called by RpgGameApp at the end of FrameTick
	void Flush() noexcept;


	// Get current mouse cursor position
	// @returns Point (X,Y) in window client
	[[nodiscard]] inline RpgPointInt GetMouseCursorPosition() const noexcept
	{
		return MouseCursorPosition;
	}

	// Get mouse cursor delta position between current and previous frame
	// @returns Delta in (X,Y) axis
	[[nodiscard]] inline RpgPointInt GetMouseCursorDeltaPosition() const noexcept
	{
		return MouseCursorDeltaPosition;
	}

	// Get current mouse wheel value
	// @returns Value in (X,Y) axis
	[[nodiscard]] inline RpgPointInt GetMouseWheelValue() const noexcept
	{
		return MouseWheelValue;
	}

	// Get current key button state
	// @param key - Key button
	// @returns Button state
	[[nodiscard]] inline RpgInputButtonState GetKeyButtonState(RpgInputKey::EButton button) const noexcept
	{
		if (!KeyButtonStates[0][button] && KeyButtonStates[1][button])
		{
			return RpgInputButtonState::PRESSED;
		}

		if (KeyButtonStates[0][button] && KeyButtonStates[1][button])
		{
			return RpgInputButtonState::DOWN;
		}

		if (KeyButtonStates[0][button] && !KeyButtonStates[1][button])
		{
			return RpgInputButtonState::RELEASED;
		}

		return RpgInputButtonState::NONE;
	}

	// Is key button pressed?
	// @param key - Key button
	// @returns TRUE if key button pressed this frame
	[[nodiscard]] inline bool IsKeyButtonPressed(RpgInputKey::EButton button) const noexcept
	{
		return GetKeyButtonState(button) == RpgInputButtonState::PRESSED;
	}

	// Is key button held down?
	// @param key - Key button
	// @returns TRUE if key button held down this frame
	[[nodiscard]] inline bool IsKeyButtonDown(RpgInputKey::EButton button) const noexcept
	{
		return GetKeyButtonState(button) == RpgInputButtonState::PRESSED || GetKeyButtonState(button) == RpgInputButtonState::DOWN;
	}

	// Is key button released?
	// @param key - Key button
	// @returns TRUE if key button released this frame
	[[nodiscard]] inline bool IsKeyButtonReleased(RpgInputKey::EButton button) const noexcept
	{
		return GetKeyButtonState(button) == RpgInputButtonState::RELEASED;
	}

};
