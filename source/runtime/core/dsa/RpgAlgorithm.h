#pragma once

#include "../RpgPlatform.h"



namespace RpgAlgorithm
{
	template<typename T>
	inline void Swap(T& out_A, T& out_B) noexcept
	{
		T temp = out_A;
		out_A = out_B;
		out_B = temp;
	}


	constexpr inline bool IsPowerOfTwo(uint32_t value) noexcept
	{
		return (value > 0) && !(value & (value - 1));
	}


	template<typename T>
	inline void Array_CopyElements(int copyCount, T* dstDataArray, int dstDataCount, int dstCopyIndex, const T* srcDataArray, int srcDataCount, int srcCopyIndex) noexcept
	{
		RPG_Check(copyCount > 0);
		RPG_Check(dstDataArray && dstDataCount > 0 && dstDataCount <= RPG_MAX_COUNT);
		RPG_CheckV(dstCopyIndex >= 0 && (dstCopyIndex + copyCount) <= dstDataCount, "RpgAlgorithm: Array copy elements dst range out of bound!");
		RPG_Check(srcDataArray && srcDataCount > 0 && srcDataCount <= RPG_MAX_COUNT);
		RPG_CheckV(srcCopyIndex >= 0 && (srcCopyIndex + copyCount) <= srcDataCount, "RpgAlgorithm: Array copy elements src range out of bound!");

		if constexpr (std::is_trivially_copyable<T>::value)
		{
			RpgPlatformMemory::MemCopy(dstDataArray + dstCopyIndex, srcDataArray + srcCopyIndex, sizeof(T) * copyCount);
		}
		else
		{
			for (int i = 0; i < copyCount; ++i)
			{
				dstDataArray[dstCopyIndex + i] = srcDataArray[srcCopyIndex + i];
			}
		}
	}


	template<typename T>
	inline void Array_ShiftElements(int shiftCount, T* dataArray, int dataCount, int dstShiftIndex, int srcShiftIndex) noexcept
	{
		RPG_Check(shiftCount > 0);
		RPG_Check(dataArray && dataCount > 0 && dataCount <= RPG_MAX_COUNT);
		RPG_CheckV(dstShiftIndex >= 0 && (dstShiftIndex + shiftCount) <= dataCount, "RpgAlgorithm: Array shift elements dst range out of bound!");
		RPG_CheckV(srcShiftIndex >= 0 && (srcShiftIndex + shiftCount) <= dataCount, "RpgAlgorithm: Array shift elements src range out of bound!");

		RpgPlatformMemory::MemMove(dataArray + dstShiftIndex, dataArray + srcShiftIndex, sizeof(T) * shiftCount);
	}


	template<typename T>
	inline void Array_RemoveElements(T* dataArray, int dataCount, int index, int count, bool bKeepOrder) noexcept
	{
		RPG_Check(dataArray && dataCount > 0 && dataCount <= RPG_MAX_COUNT);
		RPG_Check(count > 0);

		const int lastIndex = dataCount - 1;
		const int removeIndex = (index == RPG_INDEX_LAST) ? lastIndex : index;
		RPG_CheckV(removeIndex >= 0 && (removeIndex + count) <= dataCount, "RpgAlgorithm: Array remove elements range out of bound!");

		if (removeIndex == lastIndex && count == 1)
		{
			return;
		}

		if (count > 1 || bKeepOrder)
		{
			Array_ShiftElements(lastIndex - removeIndex, dataArray, dataCount, removeIndex, removeIndex + 1);
		}
		else
		{
			RPG_Check(count == 1);
			RPG_Check(!bKeepOrder);

			if constexpr (std::is_move_assignable<T>::value)
			{
				dataArray[removeIndex] = std::move(dataArray[lastIndex]);
			}
			else
			{
				dataArray[removeIndex] = dataArray[lastIndex];
			}
		}
	}


	template<typename T>
	inline void Array_InsertElements(T* dstDataArray, int dstDataCount, int dstIndex, const T* srcDataArray, int srcDataCount) noexcept
	{
		RPG_Check(dstDataArray && dstDataCount > 0 && dstDataCount <= RPG_MAX_COUNT);
		RPG_Check(srcDataArray && srcDataCount > 0 && srcDataCount <= RPG_MAX_COUNT);
		RPG_CheckV(dstIndex >= 0 && (dstIndex + srcDataCount) <= dstDataCount, "RpgAlgorithm: Array insert elements range out of bound!");

		const int lastIndex = dstDataCount - 1;

		if (dstIndex >= 0 && dstIndex < lastIndex && dstDataCount != srcDataCount)
		{
			Array_ShiftElements(srcDataCount, dstDataArray, dstDataCount, (dstIndex + srcDataCount), dstIndex);
		}

		if constexpr (std::is_trivially_copyable<T>::value)
		{
			RpgPlatformMemory::MemCopy(dstDataArray + dstIndex, srcDataArray, sizeof(T) * srcDataCount);
		}
		else
		{
			for (int i = 0; i < srcDataCount; ++i)
			{
				dstDataArray[dstIndex + i] = srcDataArray[i];
			}
		}
	}


	template<typename T>
	inline int LinearSearch_FindIndexByValue(const T* dataArray, int dataCount, const T& value) noexcept
	{
		for (int i = 0; i < dataCount; ++i)
		{
			if (dataArray[i] == value)
			{
				return i;
			}
		}

		return RPG_INDEX_INVALID;
	}


	template<typename T>
	inline int LinearSearch_FindIndexByValueFromLast(const T* dataArray, int dataCount, const T& value) noexcept
	{
		for (int i = (dataCount - 1); i >= 0; --i)
		{
			if (dataArray[i] == value)
			{
				return i;
			}
		}

		return RPG_INDEX_INVALID;
	}


	template<typename T, typename TCompare>
	inline int LinearSearch_FindIndexByCompare(const T* dataArray, int dataCount, const TCompare& compare) noexcept
	{
		for (int i = 0; i < dataCount; ++i)
		{
			if (dataArray[i] == compare)
			{
				return i;
			}
		}

		return RPG_INDEX_INVALID;
	}


	template<typename T, typename TPredicate>
	inline int LinearSearch_FindIndexByPredicate(const T* dataArray, int dataCount, TPredicate predicate) noexcept
	{
		for (int i = 0; i < dataCount; ++i)
		{
			if (predicate(dataArray[i]))
			{
				return i;
			}
		}

		return RPG_INDEX_INVALID;
	}

}; // RpgAlgorithm
