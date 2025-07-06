#include "RpgGameState.h"



RpgGameState_Intro::RpgGameState_Intro(const RpgName& in_Name) noexcept
	: RpgGameState(in_Name)
{
	bTickUpdate = true;
	DurationSeconds = 5.0f;
	DurationTimer = 0.0f;
}


void RpgGameState_Intro::OnEnterState(RpgStateMachine& stateMachine) noexcept
{
	RPG_PLATFORM_LogDebug(RpgLogGameState, "Enter state (%s)", *Name);

	DurationTimer = 0.0f;
}


void RpgGameState_Intro::OnLeaveState(RpgStateMachine& stateMachine) noexcept
{
	RPG_PLATFORM_LogDebug(RpgLogGameState, "Leave state (%s)", *Name);

	DurationTimer = 0.0f;
}


void RpgGameState_Intro::OnTickUpdateState(RpgStateMachine& stateMachine, float deltaTime)
{
	//RPG_PLATFORM_LogDebug(RpgLogGame, "TickUpdate state (%s) (DurationTimer: %.2f)", *Name, DurationTimer);

	DurationTimer += deltaTime;

	if (DurationTimer >= DurationSeconds)
	{
		stateMachine.SetState("MAIN_MENU");
	}
}
