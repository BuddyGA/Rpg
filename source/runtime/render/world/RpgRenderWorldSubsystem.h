#pragma once

#include "core/world/RpgWorld.h"



class RpgRenderWorldSubsystem : public RpgWorldSubsystem
{
public:
	bool bDebugDrawMeshBounds;
	bool bDebugDrawLightBounds;


public:
	RpgRenderWorldSubsystem() noexcept;

protected:
	virtual void PostTickUpdate() noexcept override;
	virtual void Render(int frameIndex, RpgRenderer* renderer) noexcept override;

};
