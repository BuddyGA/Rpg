#pragma once

#include "core/RpgMath.h"



namespace RpgCollisionShape
{
	enum EType : uint8_t
	{
		NONE = 0,

		SPHERE,
		BOX,
		CAPSULE,
		MESH_CONVEX,
		MESH_TRIANGLE,

		MAX_COUNT
	};
	
};

constexpr const char* k_CollisionShape_Names[RpgCollisionShape::MAX_COUNT] =
{
	"NONE",
	"SPHERE",
	"BOX",
	"CAPSULE",
	"MESH_CONVEX",
	"MESH_TRIANGLE"
};



namespace RpgCollisionChannel
{
	enum EType : uint8_t
	{
		NONE = 0,
		BLOCKER,
		TRIGGER,
		CHARACTER,
		INTERACTABLE,
		ABILITY,
		MOUSE_SELECT,

		MAX_COUNT
	};

};



namespace RpgCollisionResponse
{
	enum EType : uint8_t
	{
		NONE = 0,
		OVERLAP,
		BLOCK
	};
};
typedef RpgArrayInline<RpgCollisionResponse::EType, RpgCollisionChannel::MAX_COUNT> RpgCollisionResponseChannels;



// Default collision response channels to ignore all
const RpgCollisionResponseChannels k_CollisionResponseChannels_IgnoreAll =
{
	RpgCollisionResponse::NONE,		// NONE
	RpgCollisionResponse::NONE,		// BLOCKER
	RpgCollisionResponse::NONE,		// TRIGGER
	RpgCollisionResponse::NONE,		// CHARACTER
	RpgCollisionResponse::NONE,		// INTERACTABLE
	RpgCollisionResponse::NONE,		// ABILITY
	RpgCollisionResponse::NONE,		// MOUSE_SELECT
};


// Default collision response channels for blocker (floor, wall, pillar, etc)
const RpgCollisionResponseChannels k_CollisionResponseChannels_Blocker =
{
	RpgCollisionResponse::NONE,		// NONE
	RpgCollisionResponse::NONE,		// BLOCKER
	RpgCollisionResponse::NONE,		// TRIGGER
	RpgCollisionResponse::BLOCK,	// CHARACTER
	RpgCollisionResponse::NONE,		// INTERACTABLE
	RpgCollisionResponse::BLOCK,	// ABILITY
	RpgCollisionResponse::BLOCK,	// MOUSE_SELECT
};


// Default collision response channels for character
const RpgCollisionResponseChannels k_CollisionResponseChannels_Character =
{
	RpgCollisionResponse::NONE,		// NONE
	RpgCollisionResponse::BLOCK,	// BLOCKER
	RpgCollisionResponse::OVERLAP,	// TRIGGER
	RpgCollisionResponse::BLOCK,	// CHARACTER
	RpgCollisionResponse::NONE,		// INTERACTABLE
	RpgCollisionResponse::OVERLAP,	// ABILITY
	RpgCollisionResponse::BLOCK,	// MOUSE_SELECT
};




struct RpgCollisionContactResult
{
	RpgVector3 SeparationDirection;
	RpgVector3 ContactPoint;
	float PenetrationDepth{ 0.0f };
};
