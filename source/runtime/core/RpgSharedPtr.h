#pragma once

#include "RpgPlatform.h"



template<typename T>
struct RpgRefCount
{
	T* Object{ nullptr };
	SDL_AtomicInt SharedCount;
	SDL_AtomicInt WeakCount;
};



template<typename T>
class RpgSharedPtr
{
private:
	RpgRefCount<T>* Ref;


public:
	explicit RpgSharedPtr(T* in_Obj = nullptr) noexcept
	{
		Ref = nullptr;

		if (in_Obj)
		{
			Ref = new RpgRefCount<T>();
			Ref->Object = in_Obj;
			SDL_SetAtomicInt(&Ref->SharedCount, 1);
			SDL_SetAtomicInt(&Ref->WeakCount, 0);
		}
	}


	RpgSharedPtr(const RpgSharedPtr& other) noexcept
		: Ref(other.Ref)
	{
		if (Ref)
		{
			SDL_AddAtomicInt(&Ref->SharedCount, 1);
		}
	}


	RpgSharedPtr(RpgSharedPtr&& other) noexcept
		: Ref(other.Ref)
	{
		other.Ref = nullptr;
	}


	~RpgSharedPtr() noexcept
	{
		Release();
	}


public:
	inline RpgSharedPtr& operator=(const RpgSharedPtr& rhs) noexcept
	{
		if (this != &rhs)
		{
			Release();
			Ref = rhs.Ref;

			if (Ref)
			{
				SDL_AddAtomicInt(&Ref->SharedCount, 1);
			}
		}

		return *this;
	}


	inline RpgSharedPtr& operator=(RpgSharedPtr&& rhs) noexcept
	{
		if (this != &rhs)
		{
			Release();
			Ref = rhs.Ref;
			rhs.Ref = nullptr;
		}

		return *this;
	}


	inline bool operator==(const RpgSharedPtr& rhs) const noexcept
	{
		return Ref == rhs.Ref;
	}


	inline bool operator!=(const RpgSharedPtr& rhs) const noexcept
	{
		return !(*this == rhs);
	}


	inline T* operator->() noexcept
	{
		return Ref ? Ref->Object : nullptr;
	}

	inline const T* operator->() const noexcept
	{
		return Ref ? Ref->Object : nullptr;
	}


	inline operator bool() const noexcept
	{
		return Ref && Ref->Object;
	}


public:
	[[nodiscard]] inline bool IsValid() const noexcept
	{
		return Ref && Ref->Object;
	}


	inline void Release() noexcept
	{
		if (Ref)
		{
			// SDL_AddAtomicInt returns previous value in SDL3
			// If return == 1, we were the last user
			if (SDL_AddAtomicInt(&Ref->SharedCount, -1) == 1)
			{
				delete Ref->Object;
				Ref->Object = nullptr;

				if (SDL_GetAtomicInt(&Ref->WeakCount) == 0)
				{
					delete Ref;
				}
			}
		}

		Ref = nullptr;
	}


	[[nodiscard]] inline T* Get() noexcept
	{
		return Ref ? Ref->Object : nullptr;
	}

	[[nodiscard]] inline const T* Get() const noexcept
	{
		return Ref ? Ref->Object : nullptr;
	}

	[[nodiscard]] inline int GetRefCount() const noexcept
	{
		return Ref ? SDL_GetAtomicInt(&Ref->SharedCount) : 0;
	}


	template<typename>
	friend class RpgWeakPtr;

};




template<typename T>
class RpgWeakPtr
{
private:
	RpgRefCount<T>* Ref;


public:
	RpgWeakPtr(const RpgSharedPtr<T>& shared) noexcept
	{
		Ref = shared.Ref;

		if (Ref)
		{
			SDL_AddAtomicInt(&Ref->WeakCount, 1);
		}
	}


	RpgWeakPtr(const RpgWeakPtr& other) noexcept
	{
		Ref = other.Ref;

		if (Ref)
		{
			SDL_AddAtomicInt(&Ref->WeakCount, 1);
		}
	}


	RpgWeakPtr(RpgWeakPtr&& other) noexcept
	{
		Ref = other.Ref;
		other.Ref = nullptr;
	}


	~RpgWeakPtr() noexcept
	{
		Release();
	}


public:
	inline RpgWeakPtr& operator=(const RpgSharedPtr<T>& rhs) noexcept
	{
		Release();
		Ref = rhs.Ref;

		if (Ref)
		{
			SDL_AddAtomicInt(&Ref->WeakCount, 1);
		}

		return *this;
	}


	inline RpgWeakPtr& operator=(const RpgWeakPtr& rhs) noexcept
	{
		if (this != &rhs)
		{
			Release();
			Ref = rhs.Ref;

			if (Ref)
			{
				SDL_AddAtomicInt(&Ref->WeakCount, 1);
			}
		}

		return *this;
	}


	inline RpgWeakPtr& operator=(RpgWeakPtr&& rhs) noexcept
	{
		if (this != &rhs)
		{
			Release();
			Ref = rhs.Ref;
			rhs.Ref = nullptr;
		}

		return *this;
	}


	inline bool operator==(const RpgWeakPtr& rhs) const noexcept
	{
		return Ref == rhs.Ref;
	}


	inline bool operator!=(const RpgWeakPtr& rhs) const noexcept
	{
		return !(*this == rhs);
	}


public:
	inline void Release() noexcept
	{
		if (Ref)
		{
			// SDL_AddAtomicInt returns previous value in SDL3
			// If return == 1, we were the last user
			if (SDL_AddAtomicInt(&Ref->WeakCount, -1) == 1 && SDL_GetAtomicInt(&Ref->SharedCount) == 0)
			{
				RPG_PLATFORM_Check(Ref->Object == nullptr);
				delete Ref;
			}
		}

		Ref = nullptr;
	}


	[[nodiscard]] inline RpgSharedPtr<T> AsShared() const noexcept
	{
		RpgSharedPtr<T> shared;

		if (Ref == nullptr)
		{
			return shared;
		}

		int oldValue = 0;
		do
		{
			oldValue = SDL_GetAtomicInt(&Ref->SharedCount);
			if (oldValue == 0)
			{
				return shared;
			}
		}
		while (!SDL_CompareAndSwapAtomicInt(&Ref->SharedCount, oldValue, oldValue + 1));

		shared.Ref = Ref;
		RPG_PLATFORM_Check(shared.Ref && shared.Ref->Object);

		return shared;
	}


	inline bool operator==(const RpgSharedPtr<T>& rhs) const noexcept
	{
		return Ref == rhs.Ref;
	}

};
