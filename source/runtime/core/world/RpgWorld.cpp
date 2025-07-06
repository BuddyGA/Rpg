#include "RpgWorld.h"


RPG_PLATFORM_LOG_DEFINE_CATEGORY(RpgLogWorld, VERBOSITY_LOG)



RpgWorld::RpgWorld(const RpgName& name) noexcept
{
    RPG_PLATFORM_LogDebug(RpgLogWorld, "Create world (%s)", *name);

    Name = name;
    bHasStartedPlay = false;
}


RpgWorld::~RpgWorld() noexcept
{
    RPG_PLATFORM_LogDebug(RpgLogWorld, "Destroy world (%s)", *Name);

    for (int i = 0; i < RPG_COMPONENT_TYPE_MAX_COUNT; ++i)
    {
        if (ComponentStorages[i])
        {
            delete ComponentStorages[i];
            ComponentStorages[i] = nullptr;
        }
    }

    for (int i = 0; i < Subsystems.GetCount(); ++i)
    {
        RPG_PLATFORM_LogDebug(RpgLogWorld, "Destroy subsystem (%s)", *Subsystems[i]->Name);
        delete Subsystems[i];
    }
}


void RpgWorld::DispatchStartPlay() noexcept
{
    if (bHasStartedPlay)
    {
        return;
    }

    for (int i = 0; i < Subsystems.GetCount(); ++i)
    {
        Subsystems[i]->StartPlay();
    }

    for (int i = 0; i < GameObjectScripts.GetCount(); ++i)
    {
        RpgGameObjectScript* script = GameObjectScripts[i];
        RPG_PLATFORM_Check(script);

        if (!script->bHasStartedPlay)
        {
            script->StartPlay();
            script->bHasStartedPlay = true;
        }
    }

    bHasStartedPlay = true;
}


void RpgWorld::DispatchStopPlay() noexcept
{
    if (!bHasStartedPlay)
    {
        return;
    }

    for (int i = 0; i < Subsystems.GetCount(); ++i)
    {
        Subsystems[i]->StopPlay();
    }

    for (int i = 0; i < GameObjectScripts.GetCount(); ++i)
    {
        RpgGameObjectScript* script = GameObjectScripts[i];
        RPG_PLATFORM_Check(script);

        if (script->bHasStartedPlay)
        {
            script->StopPlay();
            script->bHasStartedPlay = false;
        }
    }

    bHasStartedPlay = false;
}


void RpgWorld::DispatchTickUpdate(float deltaTime) noexcept
{
    for (int i = 0; i < Subsystems.GetCount(); ++i)
    {
        Subsystems[i]->TickUpdate(deltaTime);
    }

    for (int i = 0; i < GameObjectScripts.GetCount(); ++i)
    {
        RpgGameObjectScript* script = GameObjectScripts[i];
        RPG_PLATFORM_Check(script);

        script->TickUpdate(deltaTime);
    }
}


void RpgWorld::DispatchPostTickUpdate() noexcept
{
    for (int i = 0; i < Subsystems.GetCount(); ++i)
    {
        Subsystems[i]->PostTickUpdate();
    }
}


void RpgWorld::DispatchRender(int frameIndex, RpgRenderer* renderer) noexcept
{
    for (int i = 0; i < Subsystems.GetCount(); ++i)
    {
        Subsystems[i]->Render(frameIndex, renderer);
    }
}


void RpgWorld::PostRender() noexcept
{
    for (auto it = GameObjectInfos.CreateIterator(); it; ++it)
    {
        FGameObjectInfo& gameObjectInfo = it.GetValue();
        gameObjectInfo.Flags &= ~FLAG_TransformUpdated;
    }
}


RpgGameObjectID RpgWorld::GameObject_Create(const RpgName& name, const RpgTransform& worldTransform) noexcept
{
    RPG_IsMainThread();

    RPG_PLATFORM_Assert(!name.IsEmpty());

    RPG_PLATFORM_LogDebug(RpgLogWorld, "Create game object (%s)", *name);

    const int nameId = GameObjectNames.Add();
    const int infoId = GameObjectInfos.Add();
    const int transformId = GameObjectTransforms.Add();
    RPG_PLATFORM_Check(nameId == infoId && infoId == transformId);

    GameObjectNames[nameId] = name;
    
    FGameObjectInfo& info = GameObjectInfos[infoId];
    info.ComponentTypes = 0;

    for (int i = 0; i < RPG_COMPONENT_TYPE_MAX_COUNT; ++i)
    {
        info.ComponentIndices[i] = RPG_COMPONENT_ID_INVALID;
    }

    ++info.Gen;
    info.Flags = FLAG_Allocated | FLAG_TransformUpdated;
    RPG_PLATFORM_Check(info.Gen < UINT16_MAX);

    FGameObjectTransform& transform = GameObjectTransforms[transformId];
    transform.LocalMatrix = RpgMatrixTransform();
    transform.WorldMatrix = worldTransform.ToMatrixTransform();
    transform.InverseWorldMatrix = transform.WorldMatrix.GetInverse();

    return RpgGameObjectID(this, nameId, info.Gen);
}


void RpgWorld::GameObject_Destroy(RpgGameObjectID& gameObject) noexcept
{
    RPG_IsMainThread();

    if (!GameObject_IsValid(gameObject))
    {
        return;
    }



    gameObject = RpgGameObjectID();
}
