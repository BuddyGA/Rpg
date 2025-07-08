#pragma once

#include "core/world/RpgComponent.h"
#include "../RpgPhysicsTypes.h"



class RpgPhysicsComponent_Collision
{
	RPG_COMPONENT_TYPE("Collision");

private:
	// Internal bounding AABB for broadphase
	RpgBoundingAABB Bounding;

	// - Sphere (X = Radius, Y = Radius, Z = Radius, W = Radius)
	// - Box (XYZ = Half Extents, W = 0.0f)
	// - Capsule (X = Radius, Y = HalfHeight, Z = 0.0f, W = 0.0f)
	RpgVector4 Size;

	// Collision shape
	RpgPhysicsCollision::EShape Shape;

	// Set true to update internal bounding AABB
	bool bUpdateBounding;

public:
	// Set true to enable collision
	bool bCollisionEnabled;

	// Object collision channel
	RpgPhysicsCollision::EChannel ObjectChannel;

	// Object response againts other channel
	RpgPhysicsCollisionResponseChannels ResponseChannels;


public:
	RpgPhysicsComponent_Collision() noexcept
	{
		Shape = RpgPhysicsCollision::SHAPE_NONE;
		bUpdateBounding = false;
		bCollisionEnabled = false;
		ObjectChannel = RpgPhysicsCollision::CHANNEL_NONE;
		ResponseChannels = RPG_PHYSICS_DefaultCollisionResponseChannels_IgnoreAll;
	}


	inline void Destroy() noexcept
	{
		// No heap allocation, nothing to do
	}


	inline void SetShapeAs_Sphere(float radius) noexcept
	{
		Size = RpgVector4(radius);
		Shape = RpgPhysicsCollision::SHAPE_SPHERE;
		bUpdateBounding = true;
	}


	inline void SetShapeAs_Box(RpgVector3 halfExtents) noexcept
	{
		Size = RpgVector4(halfExtents.X, halfExtents.Y, halfExtents.Z, 0.0f);
		Shape = RpgPhysicsCollision::SHAPE_BOX;
		bUpdateBounding = true;
	}


	inline void SetShapeAs_Capsule(float radius, float halfHeight) noexcept
	{
		Size = RpgVector4(radius, halfHeight, 0.0f, 0.0f);
		Shape = RpgPhysicsCollision::SHAPE_CAPSULE;
		bUpdateBounding = true;
	}


	friend class RpgPhysicsWorldSubsystem;

};
