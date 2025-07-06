#pragma once

#include "../RpgPlatform.h"


class RpgWorld;



struct RpgGameObjectID
{
private:
	class RpgWorld* World;
	int Index;
	uint16_t Gen;


private:
	RpgGameObjectID(RpgWorld* in_World, int in_Index, uint16_t in_Gen) noexcept
	{
		World = in_World;
		Index = in_Index;
		Gen = in_Gen;
	}

public:
	RpgGameObjectID() noexcept
	{
		World = nullptr;
		Index = -1;
		Gen = UINT16_MAX;
	}

	inline bool operator==(const RpgGameObjectID& rhs) const noexcept
	{
		return World == rhs.World && Index == rhs.Index && Gen == rhs.Gen;
	}

	inline bool operator!=(const RpgGameObjectID& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	inline operator bool() const noexcept
	{
		return World && Index != -1 && Gen != UINT16_MAX;
	}


	friend RpgWorld;

};



class RpgGameObjectScript
{
	RPG_NOCOPY(RpgGameObjectScript)

protected:
	RpgGameObjectID GameObject;
	RpgWorld* World;

private:
	bool bHasStartedPlay;


private:
	RpgGameObjectScript() noexcept
	{
		World = nullptr;
		bHasStartedPlay = false;
	}

public:
	virtual ~RpgGameObjectScript() noexcept = default;

protected:
	virtual void StartPlay() noexcept {}
	virtual void StopPlay() noexcept {}
	virtual void TickUpdate(float deltaTime) noexcept {}


	friend RpgWorld;

};
