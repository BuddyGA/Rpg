#pragma once

#include "core/world/RpgComponent.h"
#include "RpgAnimationTypes.h"



// ======================================================================================================================= //
// ANIMATION COMPONENT
// ======================================================================================================================= //
struct RpgAnimationComponent
{
	RPG_COMPONENT_TYPE(4, "Animation")

public:
	RpgSharedAnimationClip Clip;
	float PlayRate{ 1.0f };
	bool bLoopAnim{ false };
	bool bPauseAnim{ false };
	bool bDebugDrawSkeletonPose{ false };

private:
	RpgSharedAnimationSkeleton Skeleton;
	RpgAnimationPose FinalPose;
	float AnimTimer{ 0.0f };


public:
	inline void Reset() noexcept
	{
		Clip.Release();
		PlayRate = 1.0f;
		bLoopAnim = false;
		bPauseAnim = false;
		bDebugDrawSkeletonPose = false;

		Skeleton.Release();
		FinalPose.Clear(true);
		AnimTimer = 0.0f;
	}


	inline void SetSkeleton(const RpgSharedAnimationSkeleton& in_Skeleton) noexcept
	{
		if (Skeleton != in_Skeleton)
		{
			Skeleton = in_Skeleton;
			ResetPose();
		}
	}

	[[nodiscard]] inline const RpgSharedAnimationSkeleton& GetSkeleton() const noexcept
	{
		return Skeleton;
	}


	inline void ResetPose() noexcept
	{
		FinalPose.Clear(true);

		if (Skeleton)
		{
			FinalPose = Skeleton->GetBindPose();
		}
	}


	[[nodiscard]] inline const RpgAnimationPose& GetFinalPose() const noexcept
	{
		return FinalPose;
	}


	friend RpgAnimationWorldSubsystem;
	friend RpgAsyncTask_TickPose;

};
