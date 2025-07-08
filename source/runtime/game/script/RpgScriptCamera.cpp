#include "RpgScriptCamera.h"
#include "core/world/RpgWorld.h"
#include "input/RpgInputManager.h"
#include "../RpgGameApp.h"



RpgScriptCamera::RpgScriptCamera() noexcept
{
	States.Resize(static_cast<uint8_t>(RpgCameraMode::MAX_COUNT));
	Mode = RpgCameraMode::FREE_FLY;
	PitchMin = -80.0f;
	PitchMax = 80.0f;
	RotationSpeed = 90.0f;
	MoveSpeed = 500.0f;
}


void RpgScriptCamera::Initialize() noexcept
{
	FState& state = GetCurrentModeState();
	state.Transform.Position = RpgVector3(0.0f, 300.0f, -300.0f);
	state.Transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(70.0f, 0.0f, 0.0f);
	state.PitchValue = 70.0f;
}


void RpgScriptCamera::StartPlay() noexcept
{
}


void RpgScriptCamera::StopPlay() noexcept
{
}


void RpgScriptCamera::TickUpdate(float deltaTime) noexcept
{
	if (Mode == RpgCameraMode::FREE_FLY)
	{
		UpdateMovement_FreeFly(deltaTime);
	}
	else if (Mode == RpgCameraMode::TOP_DOWN)
	{
		UpdateMovement_TopDown(deltaTime);
	}

	World->GameObject_SetWorldTransform(GameObject, GetCurrentModeState().Transform);
}


void RpgScriptCamera::UpdateMovement_FreeFly(float deltaTime) noexcept
{
	FState& state = GetCurrentModeState();
	RpgInputManager* input = g_GameApp->GetInputManager();

	if (input->IsKeyButtonPressed(RpgInputKey::MOUSE_RIGHT))
	{
		RPG_PLATFORM_Log(RpgLogTemp, "Camera FreeFly update movement BEGIN");
		g_GameApp->SetMouseRelativeMode(true);
		state.SavedMousePos = input->GetMouseCursorPosition();
	}
	else if (input->IsKeyButtonReleased(RpgInputKey::MOUSE_RIGHT))
	{
		RPG_PLATFORM_Log(RpgLogTemp, "Camera FreeFly update movement END");
		g_GameApp->SetMouseCursorPosition(state.SavedMousePos.X, state.SavedMousePos.Y);
		g_GameApp->SetMouseRelativeMode(false);
	}

	if (input->IsKeyButtonDown(RpgInputKey::MOUSE_RIGHT))
	{
		RpgVector3 moveAxis;

		if (input->IsKeyButtonDown(RpgInputKey::KEYBOARD_W))
		{
			moveAxis.Z = 1.0f;
		}

		if (input->IsKeyButtonDown(RpgInputKey::KEYBOARD_S))
		{
			moveAxis.Z = -1.0f;
		}

		if (input->IsKeyButtonDown(RpgInputKey::KEYBOARD_D))
		{
			moveAxis.X = 1.0f;
		}

		if (input->IsKeyButtonDown(RpgInputKey::KEYBOARD_A))
		{
			moveAxis.X = -1.0f;
		}

		const RpgPointInt deltaCursorPos = input->GetMouseCursorDeltaPosition();
		state.PitchValue += 0.25f * deltaCursorPos.Y;
		state.YawValue += 0.25f * deltaCursorPos.X;
		state.PitchValue = RpgMath::Clamp(state.PitchValue, PitchMin, PitchMax);
		state.YawValue = RpgMath::ClampDegree(state.YawValue);
		state.Transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(state.PitchValue, state.YawValue, 0.0f);

		RpgVector3 moveDirection = state.Transform.GetAxisRight() * moveAxis.X;
		moveDirection += state.Transform.GetAxisUp() * moveAxis.Y;
		moveDirection += state.Transform.GetAxisForward() * moveAxis.Z;
		moveDirection.Normalize();
		moveDirection *= MoveSpeed * deltaTime;
		state.Transform.Position += moveDirection;
	}
}


void RpgScriptCamera::UpdateMovement_TopDown(float deltaTime) noexcept
{
}
