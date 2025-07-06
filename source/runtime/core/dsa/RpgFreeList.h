#pragma once

#include "RpgAlgorithm.h"


template<typename T>
class RpgFreeList
{
	static_assert(sizeof(T) >= sizeof(int), "RpgFreeList size of type <T> must be greater than sizeof(int) 4 bytes! ");

private:
	int Capacity;
	int Count;
	int NextFreeIndex;

	// For fast checking if index is valid
	bool* ValidIndexArray;

	// Actual data array
	T* DataArray;


public:
	RpgFreeList() noexcept
		: Capacity(0)
		, Count(0)
		, NextFreeIndex(RPG_INDEX_INVALID)
		, ValidIndexArray(nullptr)
		, DataArray(nullptr)
	{
	}


	RpgFreeList(const RpgFreeList& other) noexcept
		: RpgFreeList()
	{
		CopyFromOther(other);
	}


	RpgFreeList(RpgFreeList&& other) noexcept
		: Capacity(other.Capacity)
		, Count(other.Count)
		, NextFreeIndex(other.NextFreeIndex)
		, ValidIndexArray(other.ValidIndexArray)
		, DataArray(other.DataArray)
	{
		other.Capacity = 0;
		other.Count = 0;
		other.NextFreeIndex = RPG_INDEX_INVALID;
		other.FreeIndexArray = nullptr;
		other.ValidIndexArray = nullptr;
		other.DataArray = nullptr;
	}


	RpgFreeList(const std::initializer_list<T>& initializerList) noexcept
		: RpgFreeList()
	{
		CopyFromInitializerList(initializerList);
	}


	~RpgFreeList() noexcept
	{
		Clear(true);
	}


public:
	inline RpgFreeList& operator=(const RpgFreeList& rhs) noexcept
	{
		if (this != &rhs)
		{
			Clear(true);
			CopyFromOther(rhs);
		}

		return *this;
	}


	inline RpgFreeList& operator=(RpgFreeList&& rhs) noexcept
	{
		if (this != &rhs)
		{
			Clear(true);

			Capacity = rhs.Capacity;
			Count = rhs.Count;
			NextFreeIndex = rhs.NextFreeIndex;
			ValidIndexArray = rhs.ValidIndexArray;
			DataArray = rhs.DataArray;

			rhs.Capacity = 0;
			rhs.Count = 0;
			rhs.NextFreeIndex = RPG_INDEX_INVALID;
			rhs.ValidIndexArray = nullptr;
			rhs.DataArray = nullptr;
		}

		return *this;
	}


	inline RpgFreeList& operator=(const std::initializer_list<T>& rhs) noexcept
	{
		Clear(true);
		CopyFromInitializerList(rhs);

		return *this;
	}


	inline T& operator[](int index) noexcept
	{
		RPG_PLATFORM_ValidateV(IsValid(index), "RpgFreeList: Element at index %i is not valid!", index);
		return DataArray[index];
	}


	inline const T& operator[](int index) const noexcept
	{
		RPG_PLATFORM_ValidateV(IsValid(index), "RpgFreeList: Element at index %i is not valid!", index);
		return DataArray[index];
	}


private:
	inline void InternalCopyFromOther(int otherCapacity, int otherCount, int otherNextFreeIndex, const bool* otherValidIndexArray, const T* otherDataArray) noexcept
	{
		Reserve(otherCapacity);
		Count = otherCount;
		NextFreeIndex = otherNextFreeIndex;

		// Copy valid index array
		RpgPlatformMemory::MemCopy(ValidIndexArray, otherValidIndexArray, sizeof(bool) * Capacity);

		// Copy to get the value of first 4 bytes (the NextFreeIndex) from each empty element
		RpgPlatformMemory::MemCopy(DataArray, otherDataArray, sizeof(T) * Capacity);

		// (Non-POD) actual deep copy for valid element only
		if constexpr (!std::is_trivially_copyable<T>::value)
		{
			for (int i = 0; i < Capacity; ++i)
			{
				if (ValidIndexArray[i])
				{
					DataArray[i] = otherDataArray[i];
				}
			}
		}
	}

	inline void CopyFromOther(const RpgFreeList& other) noexcept
	{
		InternalCopyFromOther(other.Capacity, other.Count, other.NextFreeIndex, other.ValidIndexArray, other.DataArray);
	}


	inline void CopyFromInitializerList(const std::initializer_list<T>& initializerList) noexcept
	{
		const int initCount = static_cast<int>(initializerList.size());

		if (initCount == 0)
		{
			return;
		}

		Reserve(initCount);
		Count = initCount;
		RpgPlatformMemory::MemSet(ValidIndexArray, 1, sizeof(bool) * Count);

		if constexpr (std::is_trivially_copyable<T>::value)
		{
			RpgPlatformMemory::MemCopy(DataArray, initializerList.begin(), sizeof(T) * Count);
		}
		else
		{
			for (int i = 0; i < Count; ++i)
			{
				DataArray[i] = *(initializerList.begin() + i);
			}
		}
	}


public:
	[[nodiscard]] inline bool IsValid(int index) const noexcept
	{
		return Capacity > 0 ? ValidIndexArray[index] : false;
	}


	inline void Reserve(int in_Capacity) noexcept
	{
		if (Capacity >= in_Capacity)
		{
			return;
		}

		const int addedCapacity = in_Capacity - Capacity;

		ValidIndexArray = reinterpret_cast<bool*>(RpgPlatformMemory::MemRealloc(ValidIndexArray, sizeof(bool) * in_Capacity));
		RpgPlatformMemory::MemZero(ValidIndexArray + Capacity, sizeof(bool) * addedCapacity);

		DataArray = reinterpret_cast<T*>(RpgPlatformMemory::MemRealloc(DataArray, sizeof(T) * in_Capacity));
		Capacity = in_Capacity;
	}


	template<typename... TConstructorArgs>
	[[nodiscard]] inline int Add(TConstructorArgs&&... args) noexcept
	{
		int index = RPG_INDEX_INVALID;

		if (NextFreeIndex == RPG_INDEX_INVALID)
		{
			index = Count;
			Reserve(Count + 1);
		}
		else
		{
			// Use the current NextFreeIndex
			index = NextFreeIndex;

			// Set current NextFreeIndex. The value is the first 4 bytes interpreted as int
			const int* intPtr = reinterpret_cast<const int*>(DataArray + index);
			NextFreeIndex = *intPtr;
		}

		RPG_PLATFORM_Check(index != RPG_INDEX_INVALID);
		ValidIndexArray[index] = true;
		new (DataArray + index)T(std::forward<TConstructorArgs>(args)...);

		++Count;

		return index;
	}


	inline void RemoveAt(int index)
	{
		RPG_PLATFORM_ValidateV(IsValid(index), "RpgFreeList: Element at index %i is not valid!", index);

		// Call destructor if not POD
		if constexpr (!std::is_trivially_copyable<T>::value)
		{
			(DataArray + index)->~T();
		}

	#ifdef RPG_BUILD_DEBUG
		// Fill data with garbage values
		RpgPlatformMemory::MemSet(DataArray + index, 0x0000DEAD, sizeof(T));
	#endif // !RPG_BUILD_DEBUG

		// Interpret the first 4 bytes of removed element as (int) and set its value from NextFreeIndex
		int* intPtr = reinterpret_cast<int*>(DataArray + index);
		*intPtr = NextFreeIndex;

		// Set the removed index as current NextFreeIndex
		NextFreeIndex = index;
		ValidIndexArray[index] = false;

		--Count;
	}


	inline void Clear(bool bFreeMemory = false) noexcept
	{
		Count = 0;
		NextFreeIndex = RPG_INDEX_INVALID;

		if (DataArray)
		{
			// Call destructor if not POD for valid item only
			if constexpr (!std::is_trivially_copyable<T>::value)
			{
				for (int i = 0; i < Capacity; ++i)
				{
					if (ValidIndexArray[i])
					{
						(DataArray + i)->~T();
					}
				}
			}

			if (bFreeMemory)
			{
				Capacity = 0;

				RpgPlatformMemory::MemFree(ValidIndexArray);
				ValidIndexArray = nullptr;

				RpgPlatformMemory::MemFree(DataArray);
				DataArray = nullptr;
			}
			else
			{
				RpgPlatformMemory::MemZero(ValidIndexArray, sizeof(bool) * Capacity);
			}
		}
	}


	[[nodiscard]] inline T& GetAt(int index) noexcept
	{
		RPG_PLATFORM_AssertV(IsValid(index), "RpgFreeList: Element at index %i is not valid!", index);
		return DataArray[index];
	}


	[[nodiscard]] inline const T& GetAt(int index) const noexcept
	{
		RPG_PLATFORM_AssertV(IsValid(index), "RpgFreeList: Element at index %i is not valid!", index);
		return DataArray[index];
	}


	[[nodiscard]] inline int GetCapacity() const noexcept
	{
		return Capacity;
	}


	[[nodiscard]] inline int GetCount() const noexcept
	{
		return Count;
	}


	[[nodiscard]] inline bool IsEmpty() const noexcept
	{
		return Count == 0;
	}



// Iterator and ConstIterator
public:
	class Iterator
	{
		RpgFreeList* FreeList;
		int Index;


	public:
		Iterator() noexcept
			: FreeList(nullptr)
			, Index(-1)
		{
		}

		Iterator(RpgFreeList* in_FreeList) noexcept
			: FreeList(in_FreeList)
			, Index(-1)
		{
			UpdateValidIndex();
		}

	private:
		inline void UpdateValidIndex() noexcept
		{
			const int capacity = FreeList->GetCapacity();

			do
			{
				if (FreeList->IsValid(++Index))
				{
					break;
				}
			}
			while (Index < capacity);
		}

	public:
		[[nodiscard]] inline int GetIndex() const noexcept
		{
			return Index;
		}

		[[nodiscard]] inline T& GetValue() const noexcept
		{
			return FreeList->GetAt(Index);
		}

		inline Iterator& operator++() noexcept
		{
			UpdateValidIndex();
			return *this;
		}

		inline T& operator*() noexcept
		{
			return FreeList->GetAt(Index);
		}

		inline T* operator->() noexcept
		{
			return &FreeList->GetAt(Index);
		}

		inline bool operator==(const Iterator& rhs) const noexcept
		{
			return FreeList == rhs.FreeList && Index == rhs.Index;
		}

		inline bool operator!=(const Iterator& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		inline operator bool() const noexcept
		{
			return FreeList && Index >= 0 && Index < FreeList->GetCapacity();
		}

	};


	class ConstIterator
	{
		const RpgFreeList* FreeList;
		int Index;

	public:
		ConstIterator() noexcept
			: FreeList(nullptr)
			, Index(-1)
		{
		}

		ConstIterator(const RpgFreeList* in_FreeList) noexcept
			: FreeList(in_FreeList)
			, Index(-1)
		{
			UpdateValidIndex();
		}

	private:
		inline void UpdateValidIndex() noexcept
		{
			const int capacity = FreeList->GetCapacity();

			do
			{
				if (FreeList->IsValid(++Index))
				{
					break;
				}
			}
			while (Index < capacity);
		}

	public:
		[[nodiscard]] inline int GetIndex() const noexcept
		{
			return Index;
		}

		[[nodiscard]] inline const T& GetValue() const noexcept
		{
			return FreeList->GetAt(Index);
		}

		inline ConstIterator& operator++() noexcept
		{
			UpdateValidIndex();
			return *this;
		}

		inline const T& operator*() const noexcept
		{
			return FreeList->GetAt(Index);
		}

		inline const T* operator->() const noexcept
		{
			return &FreeList->GetAt(Index);
		}

		inline bool operator==(const ConstIterator& rhs) const noexcept
		{
			return FreeList == rhs.FreeList && Index == rhs.Index;
		}

		inline bool operator!=(const ConstIterator& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		inline operator bool() const noexcept
		{
			return FreeList && Index >= 0 && Index < FreeList->GetCapacity();
		}

	};


public:
	[[nodiscard]] inline Iterator CreateIterator() noexcept { return Iterator(this); }
	[[nodiscard]] inline ConstIterator CreateConstIterator() const noexcept { return ConstIterator(this); }

};
