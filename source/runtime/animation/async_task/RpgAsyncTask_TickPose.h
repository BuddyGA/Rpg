#pragma once

#include "core/RpgThreadPool.h"
#include "core/dsa/RpgArray.h"


class RpgWorld;
class RpgAnimationComponent;



class RpgAsyncTask_TickPose : public RpgThreadTask
{
public:
	const RpgWorld* World;
	float DeltaTime;
	float GlobalPlayRate;
	RpgArray<RpgAnimationComponent*> AnimationComponents;


public:
	RpgAsyncTask_TickPose() noexcept;
	virtual void Reset() noexcept override;
	virtual void Execute() noexcept override;


	virtual const char* GetTaskName() const noexcept override
	{
		return "RpgAsyncTask_TickPose";
	}

};
