#pragma once

#include "../dsa/RpgFreeList.h"
#include "RpgGameObject.h"


class RpgWorld;


#define RPG_COMPONENT_ID_INVALID		UINT16_MAX
#define RPG_COMPONENT_TYPE_MAX_COUNT	16



#define RPG_COMPONENT_TYPE(name)																	\
friend RpgWorld;																					\
private:																							\
inline static uint16_t TYPE_ID = UINT16_MAX;														\
public:																								\
static constexpr const char* TYPE_NAME = name;														\
RpgGameObjectID GameObject;




class RpgComponentStorageInterface
{
	RPG_NOCOPY(RpgComponentStorageInterface)

public:
	RpgComponentStorageInterface() noexcept = default;
	virtual ~RpgComponentStorageInterface() noexcept = default;

	virtual void Remove(int index) noexcept = 0;

};


template<typename TComponent>
class RpgComponentStorage : public RpgComponentStorageInterface
{
private:
	RpgFreeList<TComponent> Components;


public:
	RpgComponentStorage() noexcept = default;
	
	~RpgComponentStorage() noexcept
	{
		RPG_PLATFORM_LogDebug(RpgLogTemp, "Destroy component storage (%s)", TComponent::TYPE_NAME);
	}


	[[nodiscard]] inline bool IsValid(int id) const noexcept 
	{
		return Components.IsValid(id);
	}

	[[nodiscard]] inline int Add() noexcept 
	{
		return Components.Add();
	}

	virtual void Remove(int id) noexcept override
	{
		Components.RemoveAt(id);
	}

	[[nodiscard]] inline TComponent& Get(int id) noexcept
	{
		return Components.GetAt(id);
	}

	[[nodiscard]] inline const TComponent& Get(int id) const noexcept
	{
		return Components.GetAt(id);
	}

	[[nodiscard]] inline RpgFreeList<TComponent>& GetComponents() noexcept
	{
		return Components;
	}

	[[nodiscard]] inline const RpgFreeList<TComponent>& GetComponents() const noexcept
	{
		return Components;
	}

};
