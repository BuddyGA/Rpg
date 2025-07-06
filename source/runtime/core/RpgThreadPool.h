#pragma once

#include "RpgPlatform.h"



class RpgThreadTask
{
	RPG_NOCOPYMOVE(RpgThreadTask)

private:
	// [0]: Idle, [1]: Running, [2]: Done
	mutable SDL_AtomicInt State;


public:
	RpgThreadTask() noexcept
		: State()
	{
	}

	virtual ~RpgThreadTask() noexcept = default;

	virtual void Reset() noexcept
	{
		RPG_PLATFORM_AssertV(SDL_GetAtomicInt(&State) != 1, "Cannot reset while it's still running!");
		SDL_SetAtomicInt(&State, 0);
	}

	virtual void Execute() noexcept = 0;
	virtual const char* GetTaskName() const noexcept { return nullptr; }


	// Called by threadpool when submit task. Do not call this manually!
	inline void SetRunning() noexcept
	{
		SDL_SetAtomicInt(&State, 1);
	}

	// Called by threadpool worker thread when task finish executed. Do not call this manually
	inline void SetDone() noexcept
	{
		SDL_SetAtomicInt(&State, 2);
	}


	// [Block] Wait until task finished.
	// @returns None
	inline void Wait() noexcept
	{
		while (SDL_GetAtomicInt(&State) == 1);
		RPG_PLATFORM_Assert(SDL_GetAtomicInt(&State) == 2);
	}


	// Check if task is in idle state
	[[nodiscard]] inline bool IsIdle() const noexcept
	{
		return SDL_GetAtomicInt(&State) == 0;
	}

	// Check if task is in running state (submitted or probably being executed by worker thread)
	[[nodiscard]] inline bool IsRunning() const noexcept
	{
		return SDL_GetAtomicInt(&State) == 1;
	}

	// Check if task is done (worker thread has finished execute this task)
	[[nodiscard]] inline bool IsDone() const noexcept
	{
		return SDL_GetAtomicInt(&State) == 2;
	}

};


#define RPG_THREAD_TASK_WaitAll(taskArray, taskCount)	\
for (int __i = 0; __i < taskCount; ++__i)				\
{														\
	if (taskArray[__i]->IsRunning())					\
	{													\
		taskArray[__i]->Wait();							\
	}													\
}




namespace RpgThreadPool
{
	// Initialize thread pool with number worker threads at max (NumCore - numOtherDedicatedThreads - 1) depends on if there's dedicated render/audio thread
	// @param numOtherDedicatedThreads - Number of other dedicated threads
	// @returns None
	void Initialize(int numOtherDedicatedThreads = 0) noexcept;


	// Shutdown threadpool
	// @returns None
	void Shutdown() noexcept;


	// Create task
	// @returns Task
	template<typename TTask, typename...TConstructorArgs>
	[[nodiscard]] inline TTask* CreateTask(TConstructorArgs&&... args) noexcept
	{
		static_assert(std::is_base_of<RpgThreadTask, TTask>::value, "CreateTask() type of <TTask> must be derived from type <RpgThreadTask>!");
		return new TTask(std::forward<TConstructorArgs>(args)...);
	}


	// Destroy task
	// @param task - Task to destroy
	// @returns None
	template<typename TTask>
	inline void DestroyTask(TTask*& task) noexcept
	{
		static_assert(std::is_base_of<RpgThreadTask, TTask>::value, "DestroyTask() type of <TTask> must be derived from type <RpgThreadTask>!");
		RPG_PLATFORM_Assert(task && (task->IsIdle() || task->IsDone()));
		delete task;
	}


	// Submit task. 
	// @param task - Task to submit
	// @returns None
	void SubmitTasks(RpgThreadTask** tasks, int taskCount) noexcept;


	// [Block] Wait all tasks
	// @param tasks - Pointer to task data array
	// @param taskCount - Number of task count
	// @returns None
	//void WaitAllTasks(RpgThreadTask** tasks, int taskCount) noexcept;

};
