#pragma once

#include "core/world/RpgWorld.h"
#include "RpgCollisionTypes.h"



class RpgCollisionWorldSubsystem : public RpgWorldSubsystem
{
private:
	bool bTickUpdateCollision;


public:
	RpgCollisionWorldSubsystem() noexcept;

protected:
	virtual void StartPlay() noexcept override;
	virtual void StopPlay() noexcept override;
	virtual void TickUpdate(float deltaTime) noexcept override;

private:
	bool TestOverlapSphereSphere(RpgBoundingSphere first, RpgBoundingSphere second, RpgCollisionContactResult* out_Result) noexcept;
	bool TestOverlapSphereBox(RpgBoundingSphere sphere, RpgBoundingBox box, RpgCollisionContactResult* out_Result) noexcept;
	bool TestOverlapBoxBox(RpgBoundingBox first, RpgBoundingBox second, RpgCollisionContactResult* out_Result) noexcept;
	bool TestOverlapBoxSphere(RpgBoundingBox box, RpgBoundingSphere sphere, RpgCollisionContactResult* out_Result) noexcept;

};
