#include "RpgGameState.h"
#include "RpgGameApp.h"



RpgGameState_Editor::RpgGameState_Editor(const RpgName& in_Name) noexcept
	: RpgGameState(in_Name)
{
	bTickUpdate = true;
}


void RpgGameState_Editor::MouseMove(const SDL_MouseMotionEvent& e) noexcept
{
}


void RpgGameState_Editor::MouseButton(const SDL_MouseButtonEvent& e) noexcept
{
}


void RpgGameState_Editor::KeyboardButton(const SDL_KeyboardEvent& e) noexcept
{
	if (e.down)
	{
		if (e.scancode == SDL_SCANCODE_F1)
		{
		}
		else if (e.scancode == SDL_SCANCODE_F2)
		{
		}
	}
}


void RpgGameState_Editor::OnEnterState(RpgStateMachine& stateMachine) noexcept
{
	RPG_PLATFORM_LogDebug(RpgLogGameState, "Enter state (%s)", *Name);
}


void RpgGameState_Editor::OnLeaveState(RpgStateMachine& stateMachine) noexcept
{
	RPG_PLATFORM_LogDebug(RpgLogGameState, "Leave state (%s)", *Name);
}


void RpgGameState_Editor::OnTickUpdateState(RpgStateMachine& stateMachine, float deltaTime)
{
	//RPG_PLATFORM_LogDebug(RpgLogGame, "TickUpdate state (%s)", *Name);
}
