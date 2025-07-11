#pragma once

#include "core/world/RpgWorld.h"
#include "../async_task/RpgAsyncTask_Capture.h"



class RpgCameraWorldSubsystem : public RpgWorldSubsystem
{
public:
	bool bDebugDrawFrustum;


public:
	RpgCameraWorldSubsystem() noexcept;

protected:
	virtual void PostTickUpdate() noexcept override;
	virtual void Render(int frameIndex, RpgRenderer* renderer) noexcept override;

};
