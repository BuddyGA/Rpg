#pragma once

#include "core/RpgMath.h"
#include "core/dsa/RpgArray.h"
#include "core/world/RpgGameObject.h"



class RpgScriptDebugCamera : public RpgGameObjectScript
{
	RPG_GAMEOBJECT_SCRIPT("RpgScript - DebugCamera")

private:
	float PitchValue;
	float YawValue;
	RpgPointInt SavedMousePos;
	bool bInitTransform;

public:
	float PitchMin;
	float PitchMax;
	float RotationSpeed;
	float MoveSpeed;


public:
	RpgScriptDebugCamera() noexcept;

protected:
	virtual void AttachedToGameObject() noexcept override;
	virtual void TickUpdate(float deltaTime) noexcept override;

public:
	inline void GetRotationPitchYaw(float& out_Pitch, float& out_Yaw) const noexcept
	{
		out_Pitch = PitchValue;
		out_Yaw = YawValue;
	}

};
