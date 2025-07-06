#pragma once

#include "../dsa/RpgFreeList.h"
#include "RpgGameObject.h"


#define RPG_COMPONENT_ID_INVALID		-1
#define RPG_COMPONENT_TYPE_MAX_COUNT	8


#define RPG_COMPONENT_TYPE(id, name)			\
static constexpr uint16_t TYPE_ID = id;			\
static constexpr const char* TYPE_NAME = name;	\
typedef int ID;									\
public:											\
RpgGameObjectID GameObject;		




class RpgComponentStorageInterface
{
	RPG_NOCOPY(RpgComponentStorageInterface)

public:
	RpgComponentStorageInterface() noexcept = default;
	virtual ~RpgComponentStorageInterface() noexcept = default;

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

	inline void Remove(int id) noexcept 
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
