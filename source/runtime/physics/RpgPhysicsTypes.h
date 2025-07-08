#pragma once

#include "core/RpgMath.h"
#include "core/dsa/RpgArray.h"



namespace RpgPhysicsCollision
{
	enum EShape : uint8_t
	{
		SHAPE_NONE = 0,
		SHAPE_SPHERE,
		SHAPE_BOX,
		SHAPE_CAPSULE,
		SHAPE_MESH_CONVEX,
		SHAPE_MESH_TRIANGLE,
		SHAPE_MAX_COUNT
	};

	constexpr const char* SHAPE_NAMES[SHAPE_MAX_COUNT] =
	{
		"<None>",
		"Sphere",
		"Box",
		"Capsule",
		"Mesh Convex",
		"Mesh Triangle"
	};
	

	enum EChannel : uint8_t
	{
		CHANNEL_NONE = 0,
		CHANNEL_BLOCKER,
		CHANNEL_TRIGGER,
		CHANNEL_CHARACTER,
		CHANNEL_INTERACTABLE,
		CHANNEL_ABILITY,
		CHANNEL_MOUSE_SELECT,
		CHANNEL_MAX_COUNT
	};


	enum EResponse : uint8_t
	{
		RESPONSE_IGNORE = 0,
		RESPONSE_OVERLAP,
		RESPONSE_BLOCK
	};

};

typedef RpgArrayInline<RpgPhysicsCollision::EResponse, RpgPhysicsCollision::CHANNEL_MAX_COUNT> RpgPhysicsCollisionResponseChannels;



// Default collision response channels to ignore all
const RpgPhysicsCollisionResponseChannels RPG_PHYSICS_DefaultCollisionResponseChannels_IgnoreAll =
{
	RpgPhysicsCollision::RESPONSE_IGNORE,		// NONE
	RpgPhysicsCollision::RESPONSE_IGNORE,		// BLOCKER
	RpgPhysicsCollision::RESPONSE_IGNORE,		// TRIGGER
	RpgPhysicsCollision::RESPONSE_IGNORE,		// CHARACTER
	RpgPhysicsCollision::RESPONSE_IGNORE,		// INTERACTABLE
	RpgPhysicsCollision::RESPONSE_IGNORE,		// ABILITY
	RpgPhysicsCollision::RESPONSE_IGNORE,		// MOUSE_SELECT
};


// Default collision response channels for blocker (floor, wall, pillar, etc)
const RpgPhysicsCollisionResponseChannels RPG_PHYSICS_DefaultCollisionResponseChannels_Blocker =
{
	RpgPhysicsCollision::RESPONSE_IGNORE,	// NONE
	RpgPhysicsCollision::RESPONSE_IGNORE,	// BLOCKER
	RpgPhysicsCollision::RESPONSE_IGNORE,	// TRIGGER
	RpgPhysicsCollision::RESPONSE_BLOCK,	// CHARACTER
	RpgPhysicsCollision::RESPONSE_IGNORE,	// INTERACTABLE
	RpgPhysicsCollision::RESPONSE_BLOCK,	// ABILITY
	RpgPhysicsCollision::RESPONSE_BLOCK,	// MOUSE_SELECT
};


// Default collision response channels for character
const RpgPhysicsCollisionResponseChannels RPG_PHYSICS_DefaultCollisionResponseChannels_Character =
{
	RpgPhysicsCollision::RESPONSE_IGNORE,	// NONE
	RpgPhysicsCollision::RESPONSE_BLOCK,	// BLOCKER
	RpgPhysicsCollision::RESPONSE_OVERLAP,	// TRIGGER
	RpgPhysicsCollision::RESPONSE_BLOCK,	// CHARACTER
	RpgPhysicsCollision::RESPONSE_IGNORE,	// INTERACTABLE
	RpgPhysicsCollision::RESPONSE_OVERLAP,	// ABILITY
	RpgPhysicsCollision::RESPONSE_BLOCK,	// MOUSE_SELECT
};




struct RpgPhysicsContactResult
{
	RpgVector3 SeparationDirection;
	RpgVector3 ContactPoint;
	float PenetrationDepth{ 0.0f };
};
