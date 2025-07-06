#pragma once

#include "core/RpgMath.h"
#include "core/RpgPlatform.h"



class RpgPlayerCamera
{
	RPG_NOCOPYMOVE(RpgPlayerCamera)

protected:
	RpgRectInt ViewportRect;
	RpgTransform Transform;
	RpgVector3 MoveAxis;
	float PitchValue;
	float YawValue;

public:
	float PitchMin;
	float PitchMax;
	float RotationSpeed;
	float MoveSpeed;


public:
	RpgPlayerCamera() noexcept;
	virtual ~RpgPlayerCamera() noexcept = default;

	virtual void Activated() noexcept {}
	virtual void Deactivated() noexcept {}
	virtual void MouseMove(const SDL_MouseMotionEvent& e) noexcept {}
	virtual void MouseButton(const SDL_MouseButtonEvent& e) noexcept {}
	virtual void KeyboardButton(const SDL_KeyboardEvent& e) noexcept {}
	void TickUpdate(float deltaTimeSeconds) noexcept;

protected:
	virtual RpgVector3 UpdateMovement(float deltaTimeSeconds) noexcept = 0;
	virtual void ViewportRectChanged() noexcept {}


public:
	inline void SetViewportRect(const RpgRectInt& rect) noexcept
	{
		ViewportRect = rect;
		ViewportRectChanged();
	}

	inline void AddRotationPitch(float in_Value) noexcept
	{
		PitchValue += in_Value;
	}

	inline void AddRotationYaw(float in_Value) noexcept
	{
		YawValue += in_Value;
	}

	inline void SetRotationPitchYaw(float in_Pitch, float in_Yaw) noexcept
	{
		PitchValue = in_Pitch;
		YawValue = in_Yaw;
	}

	inline void GetRotationPitchYaw(float& out_Pitch, float& out_Yaw) const noexcept
	{
		out_Pitch = PitchValue;
		out_Yaw = YawValue;
	}

	[[nodiscard]] inline RpgQuaternion GetRotationQuaternion() const noexcept
	{
		return Transform.Rotation;
	}

	inline void SetPosition(const RpgVector3& in_Position) noexcept
	{
		Transform.Position = in_Position;
	}

	inline RpgVector3 GetPosition() const noexcept
	{
		return Transform.Position;
	}

};




class RpgPlayerCameraFreeFly : public RpgPlayerCamera
{
private:
	RpgPointInt SavedMousePos;
	bool bUpdateMovement;

public:
	RpgPlayerCameraFreeFly() noexcept;
	virtual void MouseMove(const SDL_MouseMotionEvent& e) noexcept override;
	virtual void MouseButton(const SDL_MouseButtonEvent& e) noexcept override;
	virtual void KeyboardButton(const SDL_KeyboardEvent& e) noexcept override;

protected:
	virtual RpgVector3 UpdateMovement(float deltaTimeSeconds) noexcept override;

};




class RpgPlayerCameraTopDown : public RpgPlayerCamera
{
private:
	const RpgVector3 EDGE_SCROLL_MOVE_AXIS_VALUES[RpgRectBorders::MAX_COUNT] =
	{
		RpgVector3::ZERO,				// NONE = 0,
		RpgVector3(-1.0f, 0.0f, 1.0f),	// LEFT_TOP,
		RpgVector3(1.0f, 0.0f, 1.0f),	// RIGHT_TOP,
		RpgVector3(-1.0f, 0.0f, -1.0f),	// LEFT_BOTTOM,
		RpgVector3(1.0f, 0.0f, -1.0f),	// RIGHT_BOTTOM,
		RpgVector3::LEFT,				// LEFT,
		RpgVector3::RIGHT,				// RIGHT,
		RpgVector3::FORWARD,			// TOP,
		RpgVector3::BACKWARD			// BOTTOM,
	};

	RpgRectBorders EdgeScrollRects;
	RpgPointInt SavedMousePos;
	bool bRotating;


public:
	RpgPlayerCameraTopDown() noexcept;
	virtual void Activated() noexcept override;
	virtual void Deactivated() noexcept override;
	virtual void MouseMove(const SDL_MouseMotionEvent& e) noexcept override;
	virtual void MouseButton(const SDL_MouseButtonEvent& e) noexcept override;

protected:
	virtual RpgVector3 UpdateMovement(float deltaTimeSeconds) noexcept override;
	virtual void ViewportRectChanged() noexcept override;

};
