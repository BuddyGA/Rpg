#pragma once

#include "core/world/RpgComponent.h"
#include "../RpgAnimationTypes.h"



// ======================================================================================================================= //
// ANIMATION COMPONENT
// ======================================================================================================================= //
class RpgAnimationComponent
{
	RPG_COMPONENT_TYPE("Animation")

public:
	RpgSharedAnimationClip Clip;
	float PlayRate;
	bool bLoopAnim;
	bool bPauseAnim;
	bool bDebugDrawSkeletonPose;

private:
	RpgSharedAnimationSkeleton Skeleton;
	RpgAnimationPose FinalPose;
	float AnimTimer;


public:
	RpgAnimationComponent() noexcept
	{
		PlayRate = 1.0f;
		bLoopAnim = false;
		bPauseAnim = false;
		bDebugDrawSkeletonPose = false;
		AnimTimer = 0.0f;
	}


	inline void Destroy() noexcept
	{
		// Nothing to do
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
