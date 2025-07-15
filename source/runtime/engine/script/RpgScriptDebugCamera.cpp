#include "RpgScriptDebugCamera.h"
#include "core/world/RpgWorld.h"
#include "input/RpgInputManager.h"
#include "render/world/RpgRenderComponent.h"
#include "../RpgEngine.h"



RpgScriptDebugCamera::RpgScriptDebugCamera() noexcept
{
	Flashlight = nullptr;
	PitchValue = 0.0f;
	YawValue = 0.0f;
	bInitialized = false;

	PitchMin = -80.0f;
	PitchMax = 80.0f;
	RotationSpeed = 90.0f;
	MoveSpeed = 500.0f;
}


void RpgScriptDebugCamera::AttachedToGameObject() noexcept
{
	if (!bInitialized)
	{
		bInitialized = true;

		RpgTransform transform = World->GameObject_GetWorldTransform(GameObject);
		transform.Position = RpgVector3(0.0f, 500.0f, 0.0f);
		transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(0.0f, 0.0f, 0.0f);

		World->GameObject_SetWorldTransform(GameObject, transform);

		Flashlight = World->GameObject_AddComponent<RpgRenderComponent_Light>(GameObject);
		Flashlight->Type = RpgRenderLight::TYPE_SPOT_LIGHT;
		Flashlight->ColorIntensity = RpgColorLinear(1.0f, 1.0f, 1.0f, 2.0f);
		Flashlight->AttenuationRadius = 1600.0f;
		Flashlight->SpotInnerConeDegree = 20.0f;
		Flashlight->SpotOuterConeDegree = 40.0f;
		Flashlight->bCastShadow = false;
		Flashlight->bIsVisible = false;
	}
}


void RpgScriptDebugCamera::TickUpdate(float deltaTime) noexcept
{
	RpgInputManager& input = g_Engine->GetInputManager();
	RpgTransform transform = World->GameObject_GetWorldTransform(GameObject);

	if (input.IsKeyButtonPressed(RpgInputKey::MOUSE_RIGHT))
	{
		RPG_Log(RpgLogTemp, "Camera FreeFly update movement BEGIN");
		g_Engine->SetMouseRelativeMode(true);
		SavedMousePos = input.GetMouseCursorPosition();
	}
	else if (input.IsKeyButtonReleased(RpgInputKey::MOUSE_RIGHT))
	{
		RPG_Log(RpgLogTemp, "Camera FreeFly update movement END");
		g_Engine->SetMouseCursorPosition(SavedMousePos.X, SavedMousePos.Y);
		g_Engine->SetMouseRelativeMode(false);
	}

	if (input.IsKeyButtonDown(RpgInputKey::MOUSE_RIGHT))
	{
		RpgVector3 moveAxis;

		if (input.IsKeyButtonDown(RpgInputKey::KEYBOARD_W))
		{
			moveAxis.Z = 1.0f;
		}

		if (input.IsKeyButtonDown(RpgInputKey::KEYBOARD_S))
		{
			moveAxis.Z = -1.0f;
		}

		if (input.IsKeyButtonDown(RpgInputKey::KEYBOARD_D))
		{
			moveAxis.X = 1.0f;
		}

		if (input.IsKeyButtonDown(RpgInputKey::KEYBOARD_A))
		{
			moveAxis.X = -1.0f;
		}

		const RpgPointInt deltaCursorPos = input.GetMouseCursorDeltaPosition();
		PitchValue += 0.25f * deltaCursorPos.Y;
		YawValue += 0.25f * deltaCursorPos.X;
		PitchValue = RpgMath::Clamp(PitchValue, PitchMin, PitchMax);
		YawValue = RpgMath::ClampDegree(YawValue);
		transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(PitchValue, YawValue, 0.0f);

		RpgVector3 moveDirection = transform.GetAxisRight() * moveAxis.X;
		moveDirection += transform.GetAxisUp() * moveAxis.Y;
		moveDirection += transform.GetAxisForward() * moveAxis.Z;
		moveDirection.Normalize();
		moveDirection *= MoveSpeed * deltaTime;
		transform.Position += moveDirection;
	}

	World->GameObject_SetWorldTransform(GameObject, transform);


	if (input.IsKeyButtonPressed(RpgInputKey::KEYBOARD_F))
	{
		Flashlight->bIsVisible = !Flashlight->bIsVisible;
	}
}
