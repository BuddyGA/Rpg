#include "RpgPlayerCamera.h"
#include "RpgGameApp.h"



RpgPlayerCamera::RpgPlayerCamera() noexcept
{
	PitchValue = 0.0f;
	YawValue = 0.0f;
	PitchMin = -80.0f;
	PitchMax = 80.0f;
	RotationSpeed = 90.0f;
	MoveSpeed = 500.0f;
}


void RpgPlayerCamera::TickUpdate(float deltaTimeSeconds) noexcept
{
	PitchValue = RpgMath::Clamp(PitchValue, PitchMin, PitchMax);
	YawValue = RpgMath::ClampDegree(YawValue);
	Transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(PitchValue, YawValue, 0.0f);
	Transform.Position += UpdateMovement(deltaTimeSeconds);
}




RpgPlayerCameraFreeFly::RpgPlayerCameraFreeFly() noexcept
	: RpgPlayerCamera()
{
	bUpdateMovement = false;
}


void RpgPlayerCameraFreeFly::MouseMove(const SDL_MouseMotionEvent& e) noexcept
{
	if (bUpdateMovement)
	{
		PitchValue += 0.25f * e.yrel;
		YawValue += 0.25f * e.xrel;
	}
}


void RpgPlayerCameraFreeFly::MouseButton(const SDL_MouseButtonEvent& e) noexcept
{
	if (e.down)
	{
		if (e.button == SDL_BUTTON_RIGHT)
		{
			bUpdateMovement = true;
			g_GameApp->SetMouseRelativeMode(true);
			SavedMousePos = RpgPointInt(static_cast<int>(e.x), static_cast<int>(e.y));
		}
	}
	else
	{
		if (e.button == SDL_BUTTON_RIGHT)
		{
			bUpdateMovement = false;
			MoveAxis = RpgVector3::ZERO;
			g_GameApp->SetMouseRelativeMode(false);
			g_GameApp->SetMouseCursorPosition(SavedMousePos.X, SavedMousePos.Y);
		}
	}
}


void RpgPlayerCameraFreeFly::KeyboardButton(const SDL_KeyboardEvent& e) noexcept
{
	if (e.down)
	{
		if (bUpdateMovement)
		{
			if (e.scancode == SDL_SCANCODE_W)
			{
				MoveAxis.Z = 1.0f;
			}
			else if (e.scancode == SDL_SCANCODE_S)
			{
				MoveAxis.Z = -1.0f;
			}

			if (e.scancode == SDL_SCANCODE_D)
			{
				MoveAxis.X = 1.0f;
			}
			else if (e.scancode == SDL_SCANCODE_A)
			{
				MoveAxis.X = -1.0f;
			}
		}
	}
	else
	{
		if (e.scancode == SDL_SCANCODE_W || e.scancode == SDL_SCANCODE_S)
		{
			MoveAxis.Z = 0.0f;
		}

		if (e.scancode == SDL_SCANCODE_D || e.scancode == SDL_SCANCODE_A)
		{
			MoveAxis.X = 0.0f;
		}
	}
}


RpgVector3 RpgPlayerCameraFreeFly::UpdateMovement(float deltaTimeSeconds) noexcept
{
	RpgVector3 moveDirection = Transform.GetAxisRight() * MoveAxis.X;
	moveDirection += Transform.GetAxisUp() * MoveAxis.Y;
	moveDirection += Transform.GetAxisForward() * MoveAxis.Z;
	moveDirection.Normalize();
	moveDirection *= MoveSpeed * deltaTimeSeconds;

	return moveDirection;
}




RpgPlayerCameraTopDown::RpgPlayerCameraTopDown() noexcept
	: RpgPlayerCamera()
{
	EdgeScrollRects.BorderThickness = 4;
	EdgeScrollRects.SpaceBetweenBorder = 1;
	bRotating = false;
}


void RpgPlayerCameraTopDown::Activated() noexcept
{
	g_GameApp->ClipMouseCursor(true);
}


void RpgPlayerCameraTopDown::Deactivated() noexcept
{
	g_GameApp->ClipMouseCursor(false);
}


void RpgPlayerCameraTopDown::MouseMove(const SDL_MouseMotionEvent& e) noexcept
{
	if (bRotating)
	{
		YawValue += 0.25f * e.xrel;
		MoveAxis = RpgVector3::ZERO;
	}
	else
	{
		const RpgRectBorders::EBorder intersectBorder = EdgeScrollRects.TestIntersectBorder(RpgPointInt(static_cast<int>(e.x), static_cast<int>(e.y)));
		MoveAxis = EDGE_SCROLL_MOVE_AXIS_VALUES[intersectBorder];
	}
}


void RpgPlayerCameraTopDown::MouseButton(const SDL_MouseButtonEvent& e) noexcept
{
	if (e.down)
	{
		if (e.button == SDL_BUTTON_MIDDLE)
		{
			bRotating = true;
			g_GameApp->SetMouseRelativeMode(true);
			SavedMousePos = RpgPointInt(static_cast<int>(e.x), static_cast<int>(e.y));
		}
	}
	else
	{
		if (e.button == SDL_BUTTON_MIDDLE)
		{
			bRotating = false;
			g_GameApp->SetMouseRelativeMode(false);
			g_GameApp->SetMouseCursorPosition(SavedMousePos.X, SavedMousePos.Y);
			g_GameApp->ClipMouseCursor(true);
		}
	}
}


RpgVector3 RpgPlayerCameraTopDown::UpdateMovement(float deltaTimeSeconds) noexcept
{
	RpgVector3 moveRight = Transform.GetAxisRight() * MoveAxis.X;
	moveRight = RpgVector3::ProjectOnPlane(moveRight, RpgVector3::UP);
	moveRight.Normalize();

	RpgVector3 moveForward = Transform.GetAxisForward() * MoveAxis.Z;
	moveForward = RpgVector3::ProjectOnPlane(moveForward, RpgVector3::UP);
	moveForward.Normalize();

	RpgVector3 moveDirection = moveRight + moveForward;
	moveDirection.Normalize();
	moveDirection *= MoveSpeed * deltaTimeSeconds;

	return moveDirection;
}


void RpgPlayerCameraTopDown::ViewportRectChanged() noexcept
{
	EdgeScrollRects.UpdateRects(ViewportRect);
}
