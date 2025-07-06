#pragma once

#include "RpgAlgorithm.h"


#define RPG_ARRAY_ValidateIndex(i)		RPG_PLATFORM_ValidateV(i >= 0 && i < Count, "RpgArray: Index (%i) out of bound!", i)



template<typename T, int CAPACITY_ALIGNMENT = 1>
class RpgArray
{
	static_assert(RpgAlgorithm::IsPowerOfTwo(CAPACITY_ALIGNMENT), "RpgArray: CAPACITY_ALIGNMENT must be power of two!");

private:
	T* Data;
	int Capacity;
	int Count;


public:
	RpgArray(int in_Count = 0) noexcept
		: Data(nullptr)
		, Capacity(0)
		, Count(0)
	{
		if (in_Count > 0)
		{
			Resize(in_Count);
		}
	}


	template<typename...TConstructorArgs>
	RpgArray(int in_Count, TConstructorArgs&&... args) noexcept
		: Data(nullptr)
		, Capacity(0)
		, Count(0)
	{
		if (in_Count > 0)
		{
			ResizeConstructs(in_Count, std::forward<TConstructorArgs>(args)...);
		}
	}


	RpgArray(const T* srcData, int srcCount) noexcept
		: Data(nullptr)
		, Capacity(0)
		, Count(0)
	{
		if (srcData && srcCount > 0)
		{
			Resize(srcCount);
			RpgAlgorithm::Array_CopyElements(srcCount, Data, Count, 0, srcData, srcCount, 0);
		}
	}


	RpgArray(const std::initializer_list<T>& initializerList) noexcept
		: Data(nullptr)
		, Capacity(0)
		, Count(0)
	{
		const int srcCount = static_cast<int>(initializerList.size());
		if (srcCount > 0)
		{
			Resize(srcCount);
			RpgAlgorithm::Array_CopyElements(srcCount, Data, Count, 0, initializerList.begin(), srcCount, 0);
		}
	}


	RpgArray(const RpgArray& other) noexcept
		: Data(nullptr)
		, Capacity(0)
		, Count(0)
	{
		if (other.Count > 0)
		{
			Resize(other.Count);
			RpgAlgorithm::Array_CopyElements(other.Count, Data, Count, 0, other.Data, other.Count, 0);
		}
	}


	template<int N>
	RpgArray(const RpgArray<T, N>& other) noexcept
		: Data(nullptr)
		, Capacity(0)
		, Count(0)
	{
		if (other.GetCount() > 0)
		{
			Resize(other.GetCount());
			RpgAlgorithm::Array_CopyElements(other.GetCount(), Data, Count, 0, other.GetData(), other.GetCount(), 0);
		}
	}


	RpgArray(RpgArray&& other) noexcept
		: Data(other.Data)
		, Capacity(other.Capacity)
		, Count(other.Count)
	{
		other.Data = nullptr;
		other.Capacity = 0;
		other.Count = 0;
	}


	~RpgArray() noexcept
	{
		Clear(true);
	}


public:
	inline RpgArray& operator=(const RpgArray& rhs) noexcept
	{
		if (this != &rhs)
		{
			Clear();

			if (rhs.Count > 0)
			{
				Resize(rhs.Count);
				RpgAlgorithm::Array_CopyElements(rhs.Count, Data, Count, 0, rhs.Data, rhs.Count, 0);
			}
		}

		return *this;
	}


	inline RpgArray& operator=(RpgArray&& rhs) noexcept
	{
		if (this != &rhs)
		{
			Clear(true);
			Data = rhs.Data;
			Capacity = rhs.Capacity;
			Count = rhs.Count;
			rhs.Data = nullptr;
			rhs.Capacity = 0;
			rhs.Count = 0;
		}

		return *this;
	}


	template<int N>
	inline RpgArray& operator=(const RpgArray<T, N>& rhs) noexcept
	{
		Clear();

		if (rhs.GetCount() > 0)
		{
			Resize(rhs.GetCount());
			RpgAlgorithm::Array_CopyElements(rhs.GetCount(), Data, Count, 0, rhs.GetData(), rhs.GetCount(), 0);
		}

		return *this;
	}


	inline RpgArray& operator=(const std::initializer_list<T>& rhs) noexcept
	{
		Clear();
		const int srcCount = static_cast<int>(rhs.size());

		if (srcCount > 0)
		{
			Resize(srcCount);
			RpgAlgorithm::Array_CopyElements(srcCount, Data, Count, 0, rhs.begin(), srcCount, 0);
		}

		return *this;
	}


	inline T& operator[](int index) noexcept
	{
		RPG_ARRAY_ValidateIndex(index);
		return Data[index];
	}


	inline const T& operator[](int index) const noexcept
	{
		RPG_ARRAY_ValidateIndex(index);
		return Data[index];
	}

private:
	template<typename...TConstructorArgs>
	inline void ConstructElements(int index, int count, TConstructorArgs&&... args) noexcept
	{
		for (int i = index; i < (index + count); ++i)
		{
			RPG_ARRAY_ValidateIndex(i);
			new (Data + i)T(std::forward<TConstructorArgs>(args)...);
		}
	}


	inline void DestructElements(int index, int count) noexcept
	{
		for (int i = index; i < (index + count); ++i)
		{
			RPG_ARRAY_ValidateIndex(i);
			(Data + i)->~T();
		}
	}

public:
	[[nodiscard]] inline T* GetData(int index = 0) noexcept
	{
		if (Count == 0 && index == 0)
		{
			return nullptr;
		}

		RPG_ARRAY_ValidateIndex(index);
		return (Data + index);
	}

	[[nodiscard]] inline const T* GetData(int index = 0) const noexcept
	{
		if (Count == 0 && index == 0)
		{
			return nullptr;
		}

		RPG_ARRAY_ValidateIndex(index);
		return (Data + index);
	}


	[[nodiscard]] inline T& GetAt(int index) noexcept
	{
		RPG_ARRAY_ValidateIndex(index);
		return Data[index];
	}

	[[nodiscard]] inline const T& GetAt(int index) const noexcept
	{
		RPG_ARRAY_ValidateIndex(index);
		return Data[index];
	}

	[[nodiscard]] inline T& GetAtFirst() noexcept
	{
		return GetAt(0);
	}

	[[nodiscard]] inline const T& GetAtFirst() const noexcept
	{
		return GetAt(0);
	}

	[[nodiscard]] inline T& GetAtLast() noexcept
	{
		return GetAt(Count - 1);
	}

	[[nodiscard]] inline const T& GetAtLast() const noexcept
	{
		return GetAt(Count - 1);
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

	[[nodiscard]] inline size_t GetMemorySizeBytes_Reserved() const noexcept
	{
		return sizeof(T) * Capacity;
	}

	[[nodiscard]] inline size_t GetMemorySizeBytes_Allocated() const noexcept
	{
		return sizeof(T) * Count;
	}

	[[nodiscard]] inline int FindIndexByValue(const T& value) const noexcept
	{
		return Count > 0 ? RpgAlgorithm::LinearSearch_FindIndexByValue(Data, Count, value) : RPG_INDEX_INVALID;
	}

	[[nodiscard]] inline int FindIndexByValueFromLast(const T& value) const noexcept
	{
		return Count > 0 ? RpgAlgorithm::LinearSearch_FindIndexByValueFromLast(Data, Count, value) : RPG_INDEX_INVALID;
	}

	template<typename TCompare>
	[[nodiscard]] inline int FindIndexByCompare(const TCompare& compare) const noexcept
	{
		return Count > 0 ? RpgAlgorithm::LinearSearch_FindIndexByCompare(Data, Count, compare) : RPG_INDEX_INVALID;
	}

	template<typename TPredicate>
	[[nodiscard]] inline int FindIndexByPredicate(TPredicate predicate) const noexcept
	{
		return Count > 0 ? RpgAlgorithm::LinearSearch_FindIndexByPredicate(Data, Count, predicate) : RPG_INDEX_INVALID;
	}


	inline void Reserve(int in_Capacity) noexcept
	{
		if (Capacity >= in_Capacity)
		{
			return;
		}

		const int alignedCapacity = RpgType::Align(in_Capacity, CAPACITY_ALIGNMENT);
		RPG_PLATFORM_Check(alignedCapacity >= in_Capacity);

		T* NewData = reinterpret_cast<T*>(RpgPlatformMemory::MemRealloc(Data, sizeof(T) * alignedCapacity));
		RPG_PLATFORM_Check(NewData);

		Data = NewData;
		Capacity = alignedCapacity;
	}


	inline void Resize(int in_Count) noexcept
	{
		if (Count == in_Count)
		{
			return;
		}

		Reserve(in_Count);
		RPG_PLATFORM_Check(Capacity >= Count);

		if (Count < in_Count)
		{
			const int startIndex = Count;
			const int addCount = in_Count - Count;
			Count = in_Count;

			if constexpr (std::is_trivially_copyable<T>::value)
			{
				RpgPlatformMemory::MemZero(Data + startIndex, sizeof(T) * addCount);
			}
			else
			{
				ConstructElements(startIndex, addCount);
			}
		}
		else
		{
			const int startIndex = in_Count;
			const int removeCount = Count - in_Count;

			if constexpr (!std::is_trivially_copyable<T>::value)
			{
				DestructElements(startIndex, removeCount);
			}

			Count = in_Count;
		}
	}


	template<typename...TConstructorArgs>
	inline void ResizeConstructs(int in_Count, TConstructorArgs&&... args) noexcept
	{
		if (Count == in_Count)
		{
			return;
		}

		Reserve(in_Count);

		if (Count < in_Count)
		{
			const int startIndex = Count;
			const int addCount = in_Count - Count;
			Count = in_Count;
			ConstructElements(startIndex, addCount, std::forward<TConstructorArgs>(args)...);
		}
		else
		{
			const int startIndex = in_Count;
			const int removeCount = Count - in_Count;

			if constexpr (!std::is_trivially_copyable<T>::value)
			{
				DestructElements(startIndex, removeCount);
			}

			Count = in_Count;
		}
	}


	inline T& Add() noexcept
	{
		Resize(Count + 1);
		return Data[Count - 1];
	}


	inline void AddValue(const T& in_Value) noexcept
	{
		Resize(Count + 1);
		Data[Count - 1] = in_Value;
	}


	template<typename...TConstructorArgs>
	inline void AddConstruct(TConstructorArgs&&... args) noexcept
	{
		ResizeConstructs(Count + 1, std::forward<TConstructorArgs>(args)...);
	}


	inline bool AddUnique(const T& in_Value, int* optOut_Index = nullptr) noexcept
	{
		int index = FindIndexByValue(in_Value);
		const bool bShouldAdd = (index == RPG_INDEX_INVALID);

		if (bShouldAdd)
		{
			index = Count;
			AddValue(in_Value);
		}

		if (optOut_Index)
		{
			*optOut_Index = index;
		}

		return bShouldAdd;
	}


	inline void InsertAtRange(const T* srcData, int srcCount, int index) noexcept
	{
		if (srcData == nullptr || srcCount == 0)
		{
			return;
		}

		if (index == RPG_INDEX_LAST)
		{
			const int copyIndex = Count;
			Resize(Count + srcCount);
			RpgAlgorithm::Array_CopyElements(srcCount, Data, Count, copyIndex, srcData, srcCount, 0);
		}
		else
		{
			RPG_ARRAY_ValidateIndex(index);
			Resize(Count + srcCount);
			RpgAlgorithm::Array_InsertElements(Data, Count, index, srcData, srcCount);
		}
	}


	inline void InsertAtRange(const RpgArray& other, int index) noexcept
	{
		InsertAtRange(other.Data, other.Count, index);
	}


	template<int N>
	inline void InsertAtRange(const RpgArray<T, N>& other, int index) noexcept
	{
		InsertAtRange(other.Data, other.Count, index);
	}


	inline void InsertAtRange(const std::initializer_list<T>& initializerList, int index) noexcept
	{
		InsertAtRange(initializerList.begin(), static_cast<int>(initializerList.size()), index);
	}


	inline void InsertAt(const T& value, int index) noexcept
	{
		InsertAtRange(&value, 1, index);
	}


	inline void RemoveAtRange(int index, int count) noexcept
	{
		if (Count == 0)
		{
			return;
		}

		RPG_ARRAY_ValidateIndex(index);
		RPG_ARRAY_ValidateIndex(index + count);
		RpgAlgorithm::Array_RemoveElements(Data, Count, index, count, true);

		if constexpr (!std::is_pod<T>::value)
		{
			DestructElements(index, count);
		}

		Count -= count;
	}


	inline void RemoveAt(int index, bool bKeepOrder = true) noexcept
	{
		if (Count == 0)
		{
			return;
		}

		RPG_ARRAY_ValidateIndex(index);
		RpgAlgorithm::Array_RemoveElements(Data, Count, index, 1, bKeepOrder);

		if constexpr (!std::is_trivially_copyable<T>::value)
		{
			DestructElements(index, 1);
		}

		--Count;
	}

	inline void RemoveAtLast(bool bKeepOrder = true) noexcept
	{
		RemoveAt(Count - 1);
	}


	inline bool RemoveByValue(const T& value, bool bKeepOrder = true) noexcept
	{
		if (Count == 0)
		{
			return false;
		}

		const int index = RpgAlgorithm::LinearSearch_FindIndexByValue(Data, Count, value);
		if (index == RPG_INDEX_INVALID)
		{
			return false;
		}

		RemoveAt(index, bKeepOrder);

		return true;
	}


	template<typename TCompare>
	inline bool RemoveByCompare(const TCompare& compare, bool bKeepOrder = true) noexcept
	{
		if (Count == 0)
		{
			return false;
		}

		const int index = RpgAlgorithm::LinearSearch_FindIndexByCompare(Data, Count, compare);
		if (index == RPG_INDEX_INVALID)
		{
			return false;
		}

		RemoveAt(index, bKeepOrder);

		return true;
	}


	template<typename TPredicate>
	inline bool RemoveByPredicate(TPredicate predicate, bool bKeepOrder = true) noexcept
	{
		if (Count == 0)
		{
			return false;
		}

		const int index = RpgAlgorithm::LinearSearch_FindIndexByPredicate(Data, Count, predicate);
		if (index == RPG_INDEX_INVALID)
		{
			return false;
		}

		RemoveAt(index, bKeepOrder);

		return true;
	}


	inline void Clear(bool bFreeMemory = false) noexcept
	{
		Resize(0);

		if (Data && bFreeMemory)
		{
			RpgPlatformMemory::MemFree(Data);
			Data = nullptr;
			Capacity = 0;
		}
	}


	[[nodiscard]] inline T* begin() noexcept
	{
		return Data;
	}

	[[nodiscard]] inline const T* begin() const noexcept
	{
		return Data;
	}

	[[nodiscard]] inline T* end() noexcept
	{
		return Data + Count;
	}

	[[nodiscard]] inline const T* end() const noexcept
	{
		return Data + Count;
	}

};



template<typename T, int CAPACITY = 2>
class RpgArrayInline
{
	static_assert(std::is_trivially_copyable<T>::value, "RpgArrayInline type of <T> must be POD!");
	static_assert(CAPACITY >= 2, "RpgArrayInline CAPACITY must be greater than or equals 2!");

private:
	T Data[CAPACITY];
	int Count;


public:
	RpgArrayInline(int in_Count = 0) noexcept
		: Data()
		, Count(0)
	{
		if (in_Count > 0)
		{
			Resize(in_Count);
		}
	}


	RpgArrayInline(const std::initializer_list<T>& initializerList) noexcept
		: Data()
		, Count(0)
	{
		const int initCount = static_cast<int>(initializerList.size());
		if (initCount > 0)
		{
			Resize(initCount);
			RpgAlgorithm::Array_CopyElements(initCount, Data, Count, 0, initializerList.begin(), initCount, 0);
		}
	}


public:
	inline RpgArrayInline& operator=(const RpgArrayInline& rhs) noexcept
	{
		if (this != &rhs)
		{
			Count = 0;

			if (rhs.Count > 0)
			{
				Count = rhs.Count;
				RpgAlgorithm::Array_CopyElements(rhs.Count, Data, Count, 0, rhs.Data, rhs.Count, 0);
			}
		}

		return *this;
	}

	inline T& operator[](int index) noexcept
	{
		RPG_ARRAY_ValidateIndex(index);
		return Data[index];
	}


	inline const T& operator[](int index) const noexcept
	{
		RPG_ARRAY_ValidateIndex(index);
		return Data[index];
	}


public:
	[[nodiscard]] inline T* GetData(int index = 0) noexcept
	{
		if (Count == 0 && index == 0)
		{
			return nullptr;
		}

		RPG_ARRAY_ValidateIndex(index);
		return (Data + index);
	}

	[[nodiscard]] inline const T* GetData(int index = 0) const noexcept
	{
		if (Count == 0 && index == 0)
		{
			return nullptr;
		}

		RPG_ARRAY_ValidateIndex(index);
		return (Data + index);
	}

	[[nodiscard]] constexpr inline int GetCapacity() const noexcept
	{
		return CAPACITY;
	}

	[[nodiscard]] inline int GetCount() const noexcept
	{
		return Count;
	}

	[[nodiscard]] inline bool IsEmpty() const noexcept
	{
		return Count == 0;
	}

	[[nodiscard]] constexpr inline size_t GetMemorySizeBytes_Reserved() const noexcept
	{
		return sizeof(T) * CAPACITY;
	}

	[[nodiscard]] inline size_t GetMemorySizeBytes_Allocated() const noexcept
	{
		return sizeof(T) * Count;
	}

	[[nodiscard]] inline int FindIndexByValue(const T& value) const noexcept
	{
		return Count > 0 ? RpgAlgorithm::LinearSearch_FindIndexByValue(Data, Count, value) : RPG_INDEX_INVALID;
	}

	[[nodiscard]] inline int FindIndexByValueFromLast(const T& value) const noexcept
	{
		return Count > 0 ? RpgAlgorithm::LinearSearch_FindIndexByValueFromLast(Data, Count, value) : RPG_INDEX_INVALID;
	}

	template<typename TCompare>
	[[nodiscard]] inline int FindIndexByCompare(const TCompare& compare) const noexcept
	{
		return Count > 0 ? RpgAlgorithm::LinearSearch_FindIndexByCompare(Data, Count, compare) : RPG_INDEX_INVALID;
	}

	template<typename TPredicate>
	[[nodiscard]] inline int FindIndexByPredicate(TPredicate predicate) const noexcept
	{
		return Count > 0 ? RpgAlgorithm::LinearSearch_FindIndexByPredicate(Data, Count, predicate) : RPG_INDEX_INVALID;
	}


	inline void Resize(int in_Count) noexcept
	{
		RPG_PLATFORM_ValidateV(in_Count <= CAPACITY, "RpgArrayInline: Exceeds maximum capacity!");
		Count = in_Count;
	}


	inline T& Add() noexcept
	{
		RPG_PLATFORM_ValidateV(Count < CAPACITY, "RpgArrayInline: Exceeds maximum capacity!");
		++Count;
		return Data[Count - 1];
	}

	inline void AddValue(const T& in_Value) noexcept
	{
		Add() = in_Value;
	}

	inline bool AddUnique(const T& in_Value, int* optOut_Index = nullptr) noexcept
	{
		bool bAdded = false;
		int index = FindIndexByValue(in_Value);

		if (index == RPG_INDEX_INVALID)
		{
			index = Count;
			AddValue(in_Value);
			bAdded = true;
		}

		if (optOut_Index)
		{
			*optOut_Index = index;
		}

		return bAdded;
	}


	inline void Clear() noexcept
	{
		Count = 0;
	}


	inline void RemoveAt(int index, bool bKeepOrder = true) noexcept
	{
		if (Count == 0)
		{
			return;
		}

		RPG_ARRAY_ValidateIndex(index);
		RpgAlgorithm::Array_RemoveElements(Data, Count, index, 1, bKeepOrder);
		--Count;
	}


	inline bool RemoveByValue(const T& value, bool bKeepOrder = true) noexcept
	{
		if (Count == 0)
		{
			return false;
		}

		const int index = RpgAlgorithm::LinearSearch_FindIndexByValue(Data, Count, value);
		if (index == RPG_INDEX_INVALID)
		{
			return false;
		}

		RemoveAt(index, bKeepOrder);

		return true;
	}


	template<typename TCompare>
	inline bool RemoveByCompare(const TCompare& compare, bool bKeepOrder = true) noexcept
	{
		if (Count == 0)
		{
			return false;
		}

		const int index = RpgAlgorithm::LinearSearch_FindIndexByCompare(Data, Count, compare);
		if (index == RPG_INDEX_INVALID)
		{
			return false;
		}

		RemoveAt(index, bKeepOrder);

		return true;
	}

};
