#pragma once

#include "../RpgConfig.h"
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
	RpgArrayInline<RpgComponentStorageInterface*, RPG_COMPONENT_TYPE_MAX_COUNT> ComponentStorages;

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
		// Component index for each type
		uint16_t ComponentIndices[RPG_COMPONENT_TYPE_MAX_COUNT]{};

		// Generation number
		uint16_t Gen{ 0 };

		// Flags
		uint16_t Flags{ 0 };

		// Script indices
		int16_t ScriptIndices[RPG_GAMEOBJECT_MAX_SCRIPT]{};
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


	struct FFrameData
	{
		RpgArray<RpgGameObjectScript*> PendingDestroyScripts;
		RpgArray<int> PendingDestroyObjects;
	};

	FFrameData FrameDatas[RPG_FRAME_BUFFERING];
	int FrameIndex;


public:
	RpgWorld(const RpgName& name) noexcept;
	~RpgWorld() noexcept;

	void BeginFrame(int frameIndex) noexcept;
	void EndFrame(int frameIndex) noexcept;

	void DispatchStartPlay() noexcept;
	void DispatchStopPlay() noexcept;
	void DispatchTickUpdate(float deltaTimeSeconds) noexcept;
	void DispatchPostTickUpdate() noexcept;
	void DispatchRender(int frameIndex, RpgRenderer* renderer) noexcept;


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
	inline void Component_Register() noexcept
	{
		TComponent::TYPE_ID = ComponentStorages.GetCount();
		RPG_PLATFORM_CheckV(TComponent::TYPE_ID >= 0 && TComponent::TYPE_ID < RPG_COMPONENT_TYPE_MAX_COUNT, "RpgWorld: Exceeds maximum component type count!");
		ComponentStorages.AddValue(new RpgComponentStorage<TComponent>());
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
	[[nodiscard]] RpgGameObjectID GameObject_Create(const RpgName& name, const RpgTransform& worldTransform = RpgTransform()) noexcept;
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

	[[nodiscard]] inline RpgTransform GameObject_GetWorldTransform(RpgGameObjectID gameObject) const noexcept
	{
		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));

		RpgTransform transform;
		GameObjectTransforms[gameObject.Index].WorldMatrix.Decompose(transform.Position, transform.Rotation, transform.Scale);

		return transform;
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


public:

	template<typename TComponent>
	inline TComponent* GameObject_AddComponent(RpgGameObjectID gameObject) noexcept
	{
		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));

		FGameObjectInfo& info = GameObjectInfos[gameObject.Index];
		int index = info.ComponentIndices[TComponent::TYPE_ID];
		RPG_PLATFORM_CheckV(index == RPG_COMPONENT_ID_INVALID, "Game object (%s) already has component of type (%s)", *GameObjectNames[gameObject.Index], TComponent::TYPE_NAME);

		auto storage = Component_GetStorage<TComponent>();
		index = storage->Add();
		info.ComponentIndices[TComponent::TYPE_ID] = index;

		TComponent& data = storage->Get(index);
		data.GameObject = gameObject;

		return &data;
	}


	template<typename TComponent>
	inline void GameObject_RemoveComponent(RpgGameObjectID gameObject) noexcept
	{
		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));

		FGameObjectInfo& info = GameObjectInfos[gameObject.Index];
		const int index = info.ComponentIndices[TComponent::TYPE_ID];

		auto storage = Component_GetStorage<TComponent>();
		{
			TComponent& data = storage->Get(index);
			RPG_PLATFORM_Check(data.GameObject == gameObject);
			data.Destroy();
		}
		storage->Remove(index);

		info.ComponentIndices[TComponent::TYPE_ID] = RPG_COMPONENT_ID_INVALID;
	}


	template<typename TComponent>
	[[nodiscard]] inline TComponent* GameObject_GetComponent(RpgGameObjectID gameObject) noexcept
	{
		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));

		const FGameObjectInfo& info = GameObjectInfos[gameObject.Index];
		const int index = info.ComponentIndices[TComponent::TYPE_ID];

		if (index == RPG_COMPONENT_ID_INVALID)
		{
			return nullptr;
		}

		TComponent& data = Component_GetStorage<TComponent>()->Get(index);
		RPG_PLATFORM_Check(data.GameObject == gameObject);

		return &data;
	}

	template<typename TComponent>
	[[nodiscard]] inline const TComponent* GameObject_GetComponent(RpgGameObjectID gameObject) const noexcept
	{
		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));

		const FGameObjectInfo& info = GameObjectInfos[gameObject.Index];
		const int index = info.ComponentIndices[TComponent::TYPE_ID];

		if (index == RPG_COMPONENT_ID_INVALID)
		{
			return nullptr;
		}

		const TComponent& data = Component_GetStorage<TComponent>()->Get(index);
		RPG_PLATFORM_Check(data.GameObject == gameObject);

		return &data;
	}


private:
	inline void RemoveGameObjectScriptAtIndex(int index) noexcept
	{
		RpgGameObjectScript* script = GameObjectScripts[index];
		RPG_PLATFORM_Check(script);

		if (bHasStartedPlay)
		{
			script->StopPlay();
		}

		script->Destroy();

		FrameDatas[FrameIndex].PendingDestroyScripts.AddValue(script);
		GameObjectScripts.RemoveAt(index);
	}

public:
	template<typename TScript>
	inline TScript* GameObject_AddScript(RpgGameObjectID gameObject) noexcept
	{
		static_assert(std::is_base_of<RpgGameObjectScript, TScript>::value, "RpgWorld: GameObject_AddScript type of <TScript> must be derived from type <RpgGameObjectScript>!");

		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));
		FGameObjectInfo& info = GameObjectInfos[gameObject.Index];

		int emptyIndex = RPG_INDEX_INVALID;

		for (int i = 0; i < RPG_GAMEOBJECT_MAX_SCRIPT; ++i)
		{
			const int scriptIndex = info.ScriptIndices[i];

			if (scriptIndex != RPG_INDEX_INVALID)
			{
				if (TScript* check = dynamic_cast<TScript*>(GameObjectScripts[scriptIndex]))
				{
					RPG_PLATFORM_LogWarn(RpgLogWorld, "Script of type (%s) already exists! Ignore add script to game object (%s)", TScript::TYPE_NAME, *GameObjectNames[gameObject.Index]);
					return check;
				}
			}
			else
			{
				emptyIndex = i;
			}
		}

		RPG_PLATFORM_CheckV(emptyIndex != RPG_INDEX_INVALID, "Cannot add script into game object (%s). Exceeds maximum limit (%i) of scripts per game object!", 
			TScript::TYPE_NAME, RPG_GAMEOBJECT_MAX_SCRIPT
		);

		TScript* script = new TScript();
		script->World = this;
		script->GameObject = gameObject;

		info.ScriptIndices[emptyIndex] = GameObjectScripts.GetCount();
		GameObjectScripts.AddValue(script);

		RPG_PLATFORM_Log(RpgLogWorld, "Added script of type (%s) to game object (%s)", TScript::TYPE_NAME, *GameObjectNames[gameObject.Index]);

		script->Initialize();

		if (bHasStartedPlay)
		{
			script->StartPlay();
		}

		return script;
	}


	template<typename TScript>
	inline bool GameObject_RemoveScript(RpgGameObjectID gameObject) noexcept
	{
		static_assert(std::is_base_of<RpgGameObjectScript, TScript>::value, "RpgWorld: GameObject_AddScript type of <TScript> must be derived from type <RpgGameObjectScript>!");

		RPG_PLATFORM_Check(GameObject_IsValid(gameObject));
		FGameObjectInfo& info = GameObjectInfos[gameObject.Index];

		for (int i = 0; i < info.ScriptIndices.GetCount(); ++i)
		{
			const int scriptIndex = info.ScriptIndices[i];
			if (scriptIndex == RPG_INDEX_INVALID)
			{
				continue;
			}

			if (TScript* script = dynamic_cast<TScript*>(GameObjectScripts[scriptIndex]))
			{
				RemoveGameObjectScriptAtIndex(scriptIndex);
				info.ScriptIndices[i] = nullptr;
				RPG_PLATFORM_Log(RpgLogWorld, "Removed script of type (%s) from game object (%s)", TScript::TYPE_NAME, *GameObjectNames[gameObject.Index]);

				return;
			}
		}

		RPG_PLATFORM_LogWarn(RpgLogWorld, "Script of type (%s) not found! Ignore remove script from game object (%s)", TScript::TYPE_NAME, *GameObjectNames[gameObject.Index]);
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
