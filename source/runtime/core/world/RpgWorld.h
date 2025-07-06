#pragma once

#include "../RpgMath.h"
#include "../RpgString.h"
#include "RpgComponent.h"


RPG_PLATFORM_LOG_DECLARE_CATEGORY_EXTERN(RpgLogWorld)



class RpgWorld;
class RpgRenderer;



class RpgWorldSubsystem
{
	RPG_NOCOPY(RpgWorldSubsystem)

protected:
	RpgName Name;

private:
	RpgWorld* World;
	uint8_t UpdatePriority;


public:
	RpgWorldSubsystem() noexcept
	{
		World = nullptr;
		UpdatePriority = 0;
	}

	virtual ~RpgWorldSubsystem() noexcept = default;

protected:
	virtual void StartPlay() noexcept {}
	virtual void StopPlay() noexcept {}
	virtual void PreTickUpdate() noexcept {}
	virtual void TickUpdate(float deltaTime) noexcept {}
	virtual void PostTickUpdate() noexcept {}
	virtual void Render(int frameIndex, RpgRenderer* renderer) noexcept {}


	[[nodiscard]] inline const RpgName& GetName() const noexcept
	{
		return Name;
	}

	[[nodiscard]] inline RpgWorld* GetWorld() const noexcept
	{
		return World;
	}


	friend RpgWorld;

};




class RpgWorld
{
	RPG_NOCOPY(RpgWorld)

protected:
	RpgComponentStorageInterface* ComponentStorages[RPG_COMPONENT_TYPE_MAX_COUNT];

private:
	RpgName Name;
	bool bHasStartedPlay;

	RpgArrayInline<RpgWorldSubsystem*, 16> Subsystems;


	enum EGameObjectFlag : uint16_t
	{
		FLAG_None				= (0),
		FLAG_Allocated			= (1 << 0),
		FLAG_Loading			= (1 << 1),
		FLAG_Loaded				= (1 << 2),
		FLAG_PendingDestroy		= (1 << 3),
		FLAG_TransformUpdated	= (1 << 4),
	};

	struct FGameObjectInfo
	{
		uint32_t ComponentTypes{ 0 };
		uint32_t ComponentIndices[RPG_COMPONENT_TYPE_MAX_COUNT]{};
		uint16_t Gen{ 0 };
		uint16_t Flags{ 0 };
	};

	struct FGameObjectTransform
	{
		RpgMatrixTransform LocalMatrix;
		RpgMatrixTransform WorldMatrix;
		RpgMatrixTransform InverseWorldMatrix;
		RpgGameObjectID Parent;
	};

	RpgFreeList<RpgName> GameObjectNames;
	RpgFreeList<FGameObjectInfo> GameObjectInfos;
	RpgFreeList<FGameObjectTransform> GameObjectTransforms;

	RpgArray<RpgGameObjectScript*> GameObjectScripts;


public:
	RpgWorld(const RpgName& name) noexcept;
	~RpgWorld() noexcept;

	void DispatchStartPlay() noexcept;
	void DispatchStopPlay() noexcept;
	void DispatchTickUpdate(float deltaTimeSeconds) noexcept;
	void DispatchPostTickUpdate() noexcept;
	void DispatchRender(int frameIndex, RpgRenderer* renderer) noexcept;
	void PostRender() noexcept;


	[[nodiscard]] inline bool HasStartedPlay() const noexcept
	{
		return bHasStartedPlay;
	}



// ----------------------------------------------------------------------------------------------- //
// 	Subsystem interface
// ----------------------------------------------------------------------------------------------- //
public:
	template<typename TWorldSubsystem>
	inline void Subsystem_Add(uint8_t updatePriority = 0) noexcept
	{
		static_assert(std::is_base_of<RpgWorldSubsystem, TWorldSubsystem>::value, "RpgWorld: Add subsystem type of <TWorldSubsystem> must be derived from type <RpgWorldSubsystem>!");
		
		for (int i = 0; i < Subsystems.GetCount(); ++i)
		{
			if (TWorldSubsystem* check = dynamic_cast<TWorldSubsystem*>(Subsystems[i]))
			{
				RPG_PLATFORM_LogWarn(RpgLogWorld, "World subsystem type (%s) already exists!", *check->GetName());
				return;
			}
		}

		RpgWorldSubsystem* subsystem = new TWorldSubsystem();
		subsystem->World = this;
		subsystem->UpdatePriority = updatePriority;

		Subsystems.AddValue(subsystem);
	}


	template<typename TWorldSubsystem>
	inline TWorldSubsystem* Subsystem_Get() const noexcept
	{
		static_assert(std::is_base_of<RpgWorldSubsystem, TWorldSubsystem>::value, "RpgWorld: Get subsystem type of <TWorldSubsystem> must be derived from type <RpgWorldSubsystem>!");

		for (int i = 0; i < Subsystems.GetCount(); ++i)
		{
			if (TWorldSubsystem* check = dynamic_cast<TWorldSubsystem*>(Subsystems[i]))
			{
				return check;
			}
		}

		return nullptr;
	}



// ----------------------------------------------------------------------------------------------- //
// 	Component interface
// ----------------------------------------------------------------------------------------------- //
public:
	template<typename TComponent>
	inline void Component_AddStorage() noexcept
	{
		static_assert(TComponent::TYPE_ID < RPG_COMPONENT_TYPE_MAX_COUNT, "RpgWorld: Exceeds maximum component type count!");

		ComponentStorages[TComponent::TYPE_ID] = new RpgComponentStorage<TComponent>();
	}

	template<typename TComponent>
	[[nodiscard]] inline RpgComponentStorage<TComponent>* Component_GetStorage() noexcept
	{
		return static_cast<RpgComponentStorage<TComponent>*>(ComponentStorages[TComponent::TYPE_ID]);
	}

	template<typename TComponent>
	[[nodiscard]] inline const RpgComponentStorage<TComponent>* Component_GetStorage() const noexcept
	{
		return static_cast<const RpgComponentStorage<TComponent>*>(ComponentStorages[TComponent::TYPE_ID]);
	}

	template<typename TComponent>
	[[nodiscard]] inline RpgFreeList<TComponent>::Iterator Component_CreateIterator() noexcept
	{
		return Component_GetStorage<TComponent>()->GetComponents().CreateIterator();
	}

	template<typename TComponent>
	[[nodiscard]] inline RpgFreeList<TComponent>::ConstIterator Component_CreateConstIterator() const noexcept
	{
		return Component_GetStorage<TComponent>()->GetComponents().CreateConstIterator();
	}



// ----------------------------------------------------------------------------------------------- //
// 	GameObject interface
// ----------------------------------------------------------------------------------------------- //
public:
	[[nodiscard]] RpgGameObjectID GameObject_Create(const RpgName& name, const RpgTransform& worldTransform) noexcept;
	void GameObject_Destroy(RpgGameObjectID& gameObject) noexcept;
	

	[[nodiscard]] inline bool GameObject_IsValid(RpgGameObjectID gameObject) const noexcept
	{
		if (!gameObject || gameObject.World != this || !GameObjectInfos.IsValid(gameObject.Index))
		{
			return false;
		}

		const FGameObjectInfo& info = GameObjectInfos[gameObject.Index];
		return info.Gen == gameObject.Gen && !(info.Flags & FLAG_PendingDestroy);
	}


	inline void GameObject_SetWorldTransform(RpgGameObjectID gameObject, const RpgTransform& worldTransform) noexcept
	{
		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));

		FGameObjectTransform& transform = GameObjectTransforms[gameObject.Index];
		transform.WorldMatrix = worldTransform.ToMatrixTransform();

		GameObjectInfos[gameObject.Index].Flags |= FLAG_TransformUpdated;
	}


	[[nodiscard]] inline const RpgName& GameObject_GetName(RpgGameObjectID gameObject) const noexcept
	{
		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));
		return GameObjectNames[gameObject.Index];
	}


	[[nodiscard]] inline const RpgMatrixTransform& GameObject_GetWorldTransformMatrix(RpgGameObjectID gameObject) const noexcept
	{
		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));
		return GameObjectTransforms[gameObject.Index].WorldMatrix;
	}


	template<typename TComponent>
	[[nodiscard]] inline bool GameObject_HasComponent(RpgGameObjectID gameObject) const noexcept
	{
		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));
		return GameObjectInfos[gameObject.Index].ComponentTypes & (1 << TComponent::TYPE_ID);
	}


	template<typename TComponent>
	inline TComponent& GameObject_AddComponent(RpgGameObjectID gameObject) noexcept
	{
		RPG_PLATFORM_Check(!GameObject_HasComponent<TComponent>(gameObject));
		FGameObjectInfo& info = GameObjectInfos[gameObject.Index];
		info.ComponentTypes |= (1 << TComponent::TYPE_ID);

		auto storage = Component_GetStorage<TComponent>();
		const TComponent::ID id = storage->Add();
		RPG_PLATFORM_Check(info.ComponentIndices[TComponent::TYPE_ID] == RPG_COMPONENT_ID_INVALID);
		info.ComponentIndices[TComponent::TYPE_ID] = id;

		TComponent& data = storage->Get(id);
		data.Reset();
		data.GameObject = gameObject;

		return data;
	}


	template<typename TComponent>
	inline void GameObject_RemoveComponent(RpgGameObjectID gameObject) noexcept
	{
		RPG_PLATFORM_Check(GameObject_HasComponent<TComponent>(gameObject));

		FGameObjectInfo& info = GameObjectInfos[gameObject.Index];
		const TComponent::ID id = info.ComponentIndices[TComponent::TYPE_ID];
		RPG_PLATFORM_Check(id != RPG_COMPONENT_ID_INVALID);

		auto storage = Component_GetStorage<TComponent>();
		RPG_PLATFORM_Check(storage->IsValid(id));
		TComponent& data = storage->Get(id);
		RPG_PLATFORM_Check(data.GameObject == gameObject);
		data.Reset();
		storage->Remove(id);

		info.ComponentTypes &= ~(1 << TComponent::TYPE_ID);
		info.ComponentIndices[TComponent::TYPE_ID] = RPG_COMPONENT_ID_INVALID;
	}


	template<typename TComponent>
	[[nodiscard]] inline TComponent* GameObject_GetComponent(RpgGameObjectID gameObject) noexcept
	{
		if (!GameObject_HasComponent<TComponent>(gameObject))
		{
			return nullptr;
		}

		const FGameObjectInfo& info = GameObjectInfos[gameObject.Index];
		const TComponent::ID id = info.ComponentIndices[TComponent::TYPE_ID];
		RPG_PLATFORM_Check(id != RPG_COMPONENT_ID_INVALID);

		auto storage = Component_GetStorage<TComponent>();
		RPG_PLATFORM_Check(storage->IsValid(id));
		TComponent& data = storage->Get(id);
		RPG_PLATFORM_Check(data.GameObject == gameObject);

		return &data;
	}

	template<typename TComponent>
	[[nodiscard]] inline const TComponent* GameObject_GetComponent(RpgGameObjectID gameObject) const noexcept
	{
		if (!GameObject_HasComponent<TComponent>(gameObject))
		{
			return nullptr;
		}

		const FGameObjectInfo& info = GameObjectInfos[gameObject.Index];
		const TComponent::ID id = info.ComponentIndices[TComponent::TYPE_ID];
		RPG_PLATFORM_Check(id != RPG_COMPONENT_ID_INVALID);

		auto storage = Component_GetStorage<TComponent>();
		RPG_PLATFORM_Check(storage->IsValid(id));
		const TComponent& data = storage->Get(id);
		RPG_PLATFORM_Check(data.GameObject == gameObject);

		return &data;
	}


	[[nodiscard]] inline int GameObject_GetCount() const noexcept
	{
		return GameObjectInfos.GetCount();
	}

	[[nodiscard]] inline bool GameObject_IsTransformUpdated(RpgGameObjectID gameObject) const noexcept
	{
		RPG_PLATFORM_Check(gameObject);
		return GameObjectInfos[gameObject.Index].Flags & FLAG_TransformUpdated;
	}

};
