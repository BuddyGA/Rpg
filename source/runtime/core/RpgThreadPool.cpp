#include "RpgThreadPool.h"
#include "dsa/RpgArray.h"



namespace RpgThreadPool
{
	class FTaskQueue
	{
	private:
		RpgArray<RpgThreadTask*> Pool;
		SDL_Mutex* Mutex;


	public:
		FTaskQueue() noexcept
			: Mutex(nullptr)
		{
		}

		inline void Initialize() noexcept
		{
			Mutex = SDL_CreateMutex();
		}

		inline void PushTasks(RpgThreadTask** tasks, int count) noexcept
		{
			SDL_LockMutex(Mutex);
			Pool.InsertAtRange(tasks, count, RPG_INDEX_LAST);
			SDL_UnlockMutex(Mutex);
		}

		inline RpgThreadTask* PopTask() noexcept
		{
			SDL_LockMutex(Mutex);
			RpgThreadTask* task = nullptr;

			if (Pool.GetCount() > 0)
			{
				task = Pool[0];
				Pool.RemoveAt(0);
			}

			SDL_UnlockMutex(Mutex);

			return task;
		}

	};


	static SDL_Semaphore* SignalSemaphore;
	static FTaskQueue TaskQueue;


	struct FThreadWorker
	{
		SDL_Thread* Handle{ nullptr };
		SDL_AtomicInt IsRunning{};
	};
	static RpgArrayInline<FThreadWorker, 32> ThreadWorkers;

	static bool bInitialized;


	static int ThreadWorkerMain(void* data) noexcept
	{
		RpgThreadPool::FThreadWorker* worker = reinterpret_cast<RpgThreadPool::FThreadWorker*>(data);
		const char* threadName = SDL_GetThreadName(worker->Handle);
		
		while (SDL_GetAtomicInt(&worker->IsRunning))
		{
			SDL_WaitSemaphore(SignalSemaphore);

			// Check if should exit
			if (!SDL_GetAtomicInt(&worker->IsRunning))
			{
				break;
			}

			RpgThreadTask* task = TaskQueue.PopTask();

			while (task)
			{
				//RPG_PLATFORM_LogDebug(RpgLogSystem, "%s execute task %s", threadName, task->GetTaskName());
				task->Execute();
				task->SetDone();

				// Check if should exit
				if (!SDL_GetAtomicInt(&worker->IsRunning))
				{
					break;
				}

				task = TaskQueue.PopTask();
			}

			//RPG_PLATFORM_LogDebug(RpgLogSystem, "%s back to wait", threadName);
		}

		RPG_Log(RpgLogSystem, "%s exit", threadName);

		return 0;
	}

};


void RpgThreadPool::Initialize(int numOtherDedicatedThreads) noexcept
{
	if (bInitialized)
	{
		return;
	}

	const int cpuCount = SDL_GetNumLogicalCPUCores();
	RPG_RuntimeErrorCheck(cpuCount >= 4, "CPU must have at least 4 cores!");

	// Exclude dedicated threads and main thread
	const int numThreadWorkers = cpuCount - numOtherDedicatedThreads - 1;
	RPG_Validate(numThreadWorkers > 1);

	RPG_Log(RpgLogSystem, "Initialize threadpool with %i worker threads", numThreadWorkers);

	SignalSemaphore = SDL_CreateSemaphore(0);
	TaskQueue.Initialize();
	ThreadWorkers.Resize(numThreadWorkers);
	char tempName[32];

	for (int i = 0; i < numThreadWorkers; ++i)
	{
		FThreadWorker& worker = ThreadWorkers[i];
		snprintf(tempName, 32, "Thread-Worker-%i", i);
		SDL_SetAtomicInt(&worker.IsRunning, 1);
		worker.Handle = SDL_CreateThread(ThreadWorkerMain, tempName, &worker);
	}
	
	bInitialized = true;
}


void RpgThreadPool::Shutdown() noexcept
{
	if (!bInitialized)
	{
		return;
	}

	RPG_Log(RpgLogSystem, "Shutdown threadpool");

	for (int t = 0; t < ThreadWorkers.GetCount(); ++t)
	{
		FThreadWorker& worker = ThreadWorkers[t];
		SDL_SetAtomicInt(&worker.IsRunning, 0);
	}

	for (int t = 0; t < ThreadWorkers.GetCount(); ++t)
	{
		SDL_SignalSemaphore(SignalSemaphore);
	}

	for (int t = 0; t < ThreadWorkers.GetCount(); ++t)
	{
		FThreadWorker& worker = ThreadWorkers[t];
		SDL_WaitThread(worker.Handle, nullptr);
		worker.Handle = nullptr;
	}

	ThreadWorkers.Clear();

	SDL_DestroySemaphore(SignalSemaphore);
	SignalSemaphore = nullptr;

	bInitialized = false;
}


void RpgThreadPool::SubmitTasks(RpgThreadTask** tasks, int taskCount) noexcept
{
	RPG_Assert(tasks && taskCount > 0);

	for (int i = 0; i < taskCount; ++i)
	{
		RpgThreadTask* task = tasks[i];
		RPG_CheckV(task->IsIdle(), "Task submitted must be on idle state!");
		task->SetRunning();
	}

	TaskQueue.PushTasks(tasks, taskCount);

	for (int i = 0; i < taskCount; ++i)
	{
		SDL_SignalSemaphore(SignalSemaphore);
	}
}
