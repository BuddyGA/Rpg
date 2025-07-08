#pragma once

#include "core/world/RpgWorld.h"
#include "../RpgPhysicsTypes.h"



class RpgPhysicsWorldSubsystem : public RpgWorldSubsystem
{
private:
	bool bTickUpdateCollision;


public:
	RpgPhysicsWorldSubsystem() noexcept;

protected:
	virtual void StartPlay() noexcept override;
	virtual void StopPlay() noexcept override;
	virtual void TickUpdate(float deltaTime) noexcept override;

private:
	bool TestOverlapSphereSphere(RpgBoundingSphere first, RpgBoundingSphere second, RpgPhysicsContactResult* out_Result) noexcept;
	bool TestOverlapSphereBox(RpgBoundingSphere sphere, RpgBoundingBox box, RpgPhysicsContactResult* out_Result) noexcept;
	bool TestOverlapBoxBox(RpgBoundingBox first, RpgBoundingBox second, RpgPhysicsContactResult* out_Result) noexcept;
	bool TestOverlapBoxSphere(RpgBoundingBox box, RpgBoundingSphere sphere, RpgPhysicsContactResult* out_Result) noexcept;

};
