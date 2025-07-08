#include "RpgAnimationWorldSubsystem.h"
#include "RpgAnimationComponent.h"
#include "render/RpgRenderer.h"
#include "render/RpgRenderer2D.h"
#include "../async_task/RpgAsyncTask_TickPose.h"



RPG_PLATFORM_LOG_DEFINE_CATEGORY(RpgLogAnimation, VERBOSITY_DEBUG)



RpgAnimationWorldSubsystem::RpgAnimationWorldSubsystem() noexcept
{
	Name = "AnimationWorldSubsystem";

	for (int i = 0; i < TASK_COUNT; ++i)
	{
		TickPoseTasks[i] = RpgThreadPool::CreateTask<RpgAsyncTask_TickPose>();
	}

	bTickAnimationPose = false;
	GlobalPlayRate = 1.0f;
	bDebugDrawSkeletonBones = false;
}


RpgAnimationWorldSubsystem::~RpgAnimationWorldSubsystem() noexcept
{
	for (int i = 0; i < TASK_COUNT; ++i)
	{
		RpgThreadPool::DestroyTask(TickPoseTasks[i]);
	}
}


void RpgAnimationWorldSubsystem::StartPlay() noexcept
{
	bTickAnimationPose = true;
}


void RpgAnimationWorldSubsystem::StopPlay() noexcept
{
	bTickAnimationPose = false;
}


void RpgAnimationWorldSubsystem::TickUpdate(float deltaTime) noexcept
{
	if (!bTickAnimationPose)
	{
		return;
	}

	RpgWorld* world = GetWorld();

	// Reset tasks
	for (int i = 0; i < TASK_COUNT; ++i)
	{
		RpgAsyncTask_TickPose* task = TickPoseTasks[i];
		task->Reset();
		task->World = world;
		task->DeltaTime = deltaTime;
		task->GlobalPlayRate = GlobalPlayRate;
	}

	// Distribute tasks
	int taskIndex = 0;

	for (auto it = world->Component_CreateIterator<RpgAnimationComponent>(); it; ++it)
	{
		RpgAsyncTask_TickPose* task = TickPoseTasks[taskIndex];
		task->AnimationComponents.AddValue(&it.GetValue());
		taskIndex = (taskIndex + 1) % TASK_COUNT;
	}

	RpgThreadPool::SubmitTasks(reinterpret_cast<RpgThreadTask**>(TickPoseTasks), TASK_COUNT);
}


void RpgAnimationWorldSubsystem::Render(int frameIndex, RpgRenderer* renderer) noexcept
{
	// wait tick pose tasks
	RPG_THREAD_TASK_WaitAll(TickPoseTasks, TASK_COUNT);


#ifndef RPG_BUILD_SHIPPING
	RpgWorld* world = GetWorld();

	if (bDebugDrawSkeletonBones)
	{
		RpgVertexPrimitiveBatchLine* debugLine = renderer->Debug_GetPrimitiveBatchLine(frameIndex, world, true);

		for (auto it = world->Component_CreateIterator<RpgAnimationComponent>(); it; ++it)
		{
			const RpgAnimationComponent& comp = it.GetValue();
			if (!comp.Skeleton)
			{
				continue;
			}
			
			const RpgMatrixTransform gameObjectWorldMatrix = world->GameObject_GetWorldTransformMatrix(comp.GameObject);
			const RpgArray<int>& boneParentIndices = comp.Skeleton->GetBoneParentIndices();
			const RpgArray<RpgMatrixTransform>& bonePoseTransforms = comp.FinalPose.GetBonePoseTransforms();
			const int boneCount = boneParentIndices.GetCount();

			for (int b = 0; b < boneCount; ++b)
			{
				const RpgVector3 bonePosition = bonePoseTransforms[b].GetPosition() * gameObjectWorldMatrix;
				debugLine->AddAABB(RpgBoundingAABB(bonePosition - 2.0f, bonePosition + 2.0f), RpgColorRGBA::RED);

				const int boneParentIndex = boneParentIndices[b];
				
				if (boneParentIndex != RPG_SKELETON_BONE_INDEX_INVALID)
				{
					const RpgVector3 boneParentPosition = bonePoseTransforms[boneParentIndex].GetPosition() * gameObjectWorldMatrix;
					debugLine->AddLine(boneParentPosition, bonePosition, RpgColorRGBA::WHITE);
				}

				// Bone name
				
			}
		}
	}
#endif // !RPG_BUILD_SHIPPING
}
