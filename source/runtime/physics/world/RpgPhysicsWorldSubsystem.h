#pragma once

#include "core/world/RpgWorld.h"
#include "../task/RpgPhysicsTask_UpdateBound.h"
#include "../task/RpgPhysicsTask_UpdateShape.h"



class RpgPhysicsWorldSubsystem : public RpgWorldSubsystem
{
private:
	RpgPhysicsTask_UpdateBound TaskUpdateBound;
	RpgPhysicsTask_UpdateShape TaskUpdateShape;
	RpgArray<RpgPhysicsCollision::FPairOverlapTest> BroadphaseCollisionPairs;
	RpgArray<RpgPhysicsCollision::FPairOverlapTest> NarrowphaseCollisionPairs;
	bool bTickUpdateCollision;
	bool bDebugDrawCollisionBound;
	bool bDebugDrawCollisionShape;


public:
	RpgPhysicsWorldSubsystem() noexcept;

protected:
	virtual void StartPlay() noexcept override;
	virtual void StopPlay() noexcept override;
	virtual void TickUpdate(float deltaTime) noexcept override;
	virtual void Render(int frameIndex, RpgRenderer* renderer) noexcept override;

};
