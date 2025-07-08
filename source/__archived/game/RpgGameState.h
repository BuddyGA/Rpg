#pragma once

#include "core/RpgStateMachine.h"


RPG_PLATFORM_LOG_DECLARE_CATEGORY_EXTERN(RpgLogGameState);



class RpgGameState : public RpgState
{
public:
	RpgGameState(const RpgName& in_Name) noexcept
		: RpgState(in_Name)
	{
	}

	virtual void MouseMove(const SDL_MouseMotionEvent& e) noexcept {}
	virtual void MouseButton(const SDL_MouseButtonEvent& e) noexcept {}
	virtual void KeyboardButton(const SDL_KeyboardEvent& e) noexcept {}

};



class RpgGameState_Intro : public RpgGameState
{
private:
	float DurationSeconds;
	float DurationTimer;

public:
	RpgGameState_Intro(const RpgName& in_Name) noexcept;
	virtual void OnEnterState(RpgStateMachine& stateMachine) noexcept override;
	virtual void OnLeaveState(RpgStateMachine& stateMachine) noexcept override;
	virtual void OnTickUpdateState(RpgStateMachine& stateMachine, float deltaTime) override;

};



class RpgGameState_MainMenu : public RpgGameState
{
public:
	RpgGameState_MainMenu(const RpgName& in_Name) noexcept;
	virtual void OnEnterState(RpgStateMachine& stateMachine) noexcept override;
	virtual void OnLeaveState(RpgStateMachine& stateMachine) noexcept override;
	virtual void OnTickUpdateState(RpgStateMachine& stateMachine, float deltaTime) override;

};



class RpgGameState_Editor : public RpgGameState
{
public:
	RpgGameState_Editor(const RpgName& in_Name) noexcept;
	virtual void MouseMove(const SDL_MouseMotionEvent& e) noexcept override;
	virtual void MouseButton(const SDL_MouseButtonEvent& e) noexcept override;
	virtual void KeyboardButton(const SDL_KeyboardEvent& e) noexcept override;
	virtual void OnEnterState(RpgStateMachine& stateMachine) noexcept override;
	virtual void OnLeaveState(RpgStateMachine& stateMachine) noexcept override;
	virtual void OnTickUpdateState(RpgStateMachine& stateMachine, float deltaTime) override;

};




class RpgGameStateMachine : public RpgStateMachine
{
public:
	RpgGameStateMachine() noexcept
	{
		RpgGameState_Intro* stateIntro = AddState<RpgGameState_Intro>("INTRO");
		RpgGameState_MainMenu* stateMainMenu = AddState<RpgGameState_MainMenu>("MAIN_MENU");

	#ifndef RPG_BUILD_SHIPPING
		RpgGameState_Editor* stateEditor = AddState<RpgGameState_Editor>("EDITOR");
	#endif // !RPG_BUILD_SHIPPING

	}

};
