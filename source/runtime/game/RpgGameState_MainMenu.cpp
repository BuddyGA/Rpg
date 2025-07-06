#include "RpgGameState.h"



RpgGameState_MainMenu::RpgGameState_MainMenu(const RpgName& in_Name) noexcept
	: RpgGameState(in_Name)
{
}


void RpgGameState_MainMenu::OnEnterState(RpgStateMachine& stateMachine) noexcept
{
	RPG_PLATFORM_LogDebug(RpgLogGameState, "Enter state (%s)", *Name);

}


void RpgGameState_MainMenu::OnLeaveState(RpgStateMachine& stateMachine) noexcept
{
	RPG_PLATFORM_LogDebug(RpgLogGameState, "Leave state (%s)", *Name);
}


void RpgGameState_MainMenu::OnTickUpdateState(RpgStateMachine& stateMachine, float deltaTime)
{
}
