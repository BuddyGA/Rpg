#pragma once

#include "core/RpgMath.h"
#include "core/dsa/RpgArray.h"
#include "core/world/RpgGameObject.h"



enum class RpgCameraMode : uint8_t
{
	FREE_FLY = 0,
	TOP_DOWN,
	MAX_COUNT
};



class RpgScriptCamera : public RpgGameObjectScript
{
	RPG_GAMEOBJECT_SCRIPT("RpgScript - Camera")

private:
	RpgRectInt ViewportRect;

	struct FState
	{
		RpgTransform Transform;
		float PitchValue{ 0.0f };
		float YawValue{ 0.0f };
		RpgPointInt SavedMousePos;
	};
	
	RpgArrayInline<FState, static_cast<uint8_t>(RpgCameraMode::MAX_COUNT)> States;
	
public:
	RpgCameraMode Mode;
	float PitchMin;
	float PitchMax;
	float RotationSpeed;
	float MoveSpeed;


public:
	RpgScriptCamera() noexcept;

protected:
	virtual void Initialize() noexcept override;
	virtual void StartPlay() noexcept override;
	virtual void StopPlay() noexcept override;
	virtual void TickUpdate(float deltaTime) noexcept override;


private:
	void UpdateMovement_FreeFly(float deltaTime) noexcept;
	void UpdateMovement_TopDown(float deltaTime) noexcept;


	[[nodiscard]] inline FState& GetCurrentModeState() noexcept
	{
		return States[static_cast<uint8_t>(Mode)];
	}

	[[nodiscard]] inline const FState& GetCurrentModeState() const noexcept
	{
		return States[static_cast<uint8_t>(Mode)];
	}

public:
	inline void SetViewportRect(const RpgRectInt& rect) noexcept
	{
		ViewportRect = rect;
	}

	inline void AddRotationPitch(float in_Value) noexcept
	{
		GetCurrentModeState().PitchValue += in_Value;
	}

	inline void AddRotationYaw(float in_Value) noexcept
	{
		GetCurrentModeState().YawValue += in_Value;
	}

	inline void SetRotationPitchYaw(float in_Pitch, float in_Yaw) noexcept
	{
		GetCurrentModeState().PitchValue = in_Pitch;
		GetCurrentModeState().YawValue = in_Yaw;
	}

	inline void GetRotationPitchYaw(float& out_Pitch, float& out_Yaw) const noexcept
	{
		out_Pitch = GetCurrentModeState().PitchValue;
		out_Yaw = GetCurrentModeState().YawValue;
	}

	[[nodiscard]] inline RpgQuaternion GetRotationQuaternion() const noexcept
	{
		return GetCurrentModeState().Transform.Rotation;
	}

	inline void SetPosition(const RpgVector3& in_Position) noexcept
	{
		GetCurrentModeState().Transform.Position = in_Position;
	}

	inline RpgVector3 GetPosition() const noexcept
	{
		return GetCurrentModeState().Transform.Position;
	}

};
