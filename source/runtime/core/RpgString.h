#pragma once

#include "dsa/RpgArray.h"



// ==================================================================================================== //
// RpgStringView
// String view. Cannot be modified, mostly used for string literal or temporary read-only string.
// WARNING: This is probably not null terminated!!
// ==================================================================================================== //
class RpgStringView
{
private:
	const char* Data;
	int Length;


public:
	RpgStringView() noexcept
		: Data(nullptr)
		, Length(0)
	{
	}

	RpgStringView(const char* in_Cstr, int in_Length) noexcept
		: Data(in_Cstr)
		, Length(in_Length)
	{
	}

	[[nodiscard]] inline const char* GetData() const noexcept
	{
		return Data;
	}


	[[nodiscard]] inline int GetLength() const noexcept
	{
		return Length;
	}


	[[nodiscard]] inline bool IsEmpty() const noexcept
	{
		return Length == 0;
	}

};




#define RPG_STRING_MAX_LENGTH			(RPG_INDEX_LAST - 1)
#define RPG_STRING_CHAR_INDEX_FIRST		0
#define RPG_STRING_CHAR_INDEX_LAST		(RPG_STRING_MAX_LENGTH - 1)
#define RPG_STRING_FORMAT_BUFFER_COUNT	4096


class RpgString
{
private:
	RpgArray<char, 16> CharArray;


public:
	RpgString() noexcept = default;
	~RpgString() noexcept = default;

	RpgString(const RpgString& other) noexcept
		: CharArray(other.CharArray)
	{
	}

	RpgString(RpgString&& other) noexcept
		: CharArray(std::move(other.CharArray))
	{
	}


	explicit RpgString(const char* cstr) noexcept
	{
		CopyFromCstr(cstr);
	}


public:
	inline RpgString& operator=(const RpgString& rhs) noexcept
	{
		if (this != &rhs)
		{
			CharArray = rhs.CharArray;
		}

		return *this;
	}


	inline RpgString& operator=(RpgString&& rhs) noexcept
	{
		if (this != &rhs)
		{
			CharArray = std::move(rhs.CharArray);
		}

		return *this;
	}


	inline RpgString& operator=(const char* rhs) noexcept
	{
		if (CharArray.GetData() != rhs)
		{
			CharArray.Clear();
			CopyFromCstr(rhs);
		}

		return *this;
	}


	inline RpgString operator+(const RpgString& rhs) const noexcept
	{
		return Append(rhs);
	}

	inline RpgString& operator+=(const RpgString& rhs) noexcept
	{
		if (this != &rhs)
		{
			AppendInPlace(rhs);
		}

		return *this;
	}


	inline RpgString operator+(const char* rhs) const noexcept
	{
		return Append(rhs);
	}


	inline char* operator*() noexcept
	{
		return CharArray.GetData();
	}


	inline const char* operator*() const noexcept
	{
		return CharArray.GetData();
	}


	inline char& operator[](int index) noexcept
	{
		return CharArray[index];
	}


	inline const char& operator[](int index) const noexcept
	{
		return CharArray[index];
	}


private:
	inline void CopyFromCstr(const char* cstr) noexcept
	{
		const int len = RpgPlatformMemory::CStringLength(cstr);
		if (len > 0)
		{
			Resize(len);
			RpgPlatformMemory::CStringCopy(CharArray.GetData(), cstr);
		}
	}


public:
	inline void Clear(bool bFreeMemory = false) noexcept
	{
		CharArray.Clear(bFreeMemory);
	}

	inline void Resize(int length) noexcept
	{
		CharArray.Resize(length + 1);
	}

	[[nodiscard]] inline char* GetData() noexcept
	{
		return CharArray.GetData();
	}

	[[nodiscard]] inline const char* GetData() const noexcept
	{
		return CharArray.GetData();
	}

	[[nodiscard]] inline int GetLength() const noexcept
	{
		return CharArray.GetData() ? RpgPlatformMemory::CStringLength(CharArray.GetData()) : 0;
	}

	[[nodiscard]] inline bool IsEmpty() const noexcept
	{
		return GetLength() == 0;
	}


	[[nodiscard]] inline bool Equals(const RpgString& other, bool bIgnoreCase = false) const noexcept
	{
		return RpgPlatformMemory::CStringCompare(CharArray.GetData(), other.CharArray.GetData(), bIgnoreCase);
	}

	[[nodiscard]] inline bool Equals(const char* cstr, bool bIgnoreCase = false) const noexcept
	{
		return RpgPlatformMemory::CStringCompare(CharArray.GetData(), cstr, bIgnoreCase);
	}


	[[nodiscard]] inline int FindFirstIndexOf(char c) const noexcept
	{
		return CharArray.FindIndexByValue(c);
	}

	[[nodiscard]] inline int FindLastIndexOf(char c) const noexcept
	{
		return CharArray.FindIndexByValueFromLast(c);
	}


	inline void AppendInPlace(const char* srcStr, int srcLength = RPG_STRING_MAX_LENGTH) noexcept
	{
		if (srcStr == nullptr || srcLength == 0)
		{
			return;
		}

		const int copyLength = (srcLength == RPG_STRING_MAX_LENGTH) ? RpgPlatformMemory::CStringLength(srcStr) : srcLength;
		const int prevLength = GetLength();
		Resize(prevLength + copyLength);
		RpgPlatformMemory::MemCopy(CharArray.GetData() + prevLength, srcStr, sizeof(char) * copyLength);
	}

	inline void AppendInPlace(const RpgString& other) noexcept
	{
		AppendInPlace(other.GetData(), other.GetLength());
	}

	[[nodiscard]] inline RpgString Append(const char* srcStr, int srcLength = RPG_STRING_MAX_LENGTH) const noexcept
	{
		RpgString temp = *this;
		temp.AppendInPlace(srcStr, srcLength);

		return temp;
	}

	[[nodiscard]] inline RpgString Append(const RpgString& other) const noexcept
	{
		RpgString temp = *this;
		temp.AppendInPlace(other);

		return temp;
	}


	inline void ReplaceInPlace(char c, char with) noexcept
	{
		const int length = GetLength();

		for (int i = 0; i < length; ++i)
		{
			if (CharArray[i] == c)
			{
				CharArray[i] = with;
			}
		}
	}

	[[nodiscard]] inline RpgString Replace(char c, char with) const noexcept
	{
		RpgString temp = *this;
		temp.ReplaceInPlace(c, with);

		return temp;
	}


	[[nodiscard]] inline RpgString Substring(int index, int length = RPG_STRING_MAX_LENGTH) const noexcept
	{
		const int len = GetLength();
		if (len == 0)
		{
			return RpgString();
		}

		RPG_PLATFORM_Validate(index >= 0 && index < len);
		RPG_PLATFORM_Validate(length > 0 && length <= RPG_STRING_MAX_LENGTH);

		const int copyLength = (length == RPG_STRING_MAX_LENGTH) ? len - index : length;

		RpgString temp;
		temp.Resize(copyLength);
		RpgPlatformMemory::MemCopy(*temp, &CharArray[index], sizeof(char) * copyLength);

		return temp;
	}


	[[nodiscard]] inline RpgStringView SubstringView(int index, int length = RPG_STRING_MAX_LENGTH) const noexcept
	{
		const int thisLength = GetLength();
		return RpgStringView(&CharArray[index], length == RPG_STRING_MAX_LENGTH ? thisLength : length);
	}


public:
	template<typename...TVarArgs>
	[[nodiscard]] static inline RpgString Format(const char* format, TVarArgs&&...args) noexcept
	{
		char buffer[RPG_STRING_FORMAT_BUFFER_COUNT];
		snprintf(buffer, RPG_STRING_FORMAT_BUFFER_COUNT, format, std::forward<TVarArgs>(args)...);

		return RpgString(buffer);
	}

};


[[nodiscard]] inline uint64_t Rpg_GetHash(const RpgString& value) noexcept
{
	return RpgPlatformMemory::CStringHash(*value);
}



#define RPG_NAME_MAX_COUNT			48

class alignas(16) RpgName
{
private:
	char CharArray[RPG_NAME_MAX_COUNT];


public:
	RpgName() noexcept
		: CharArray()
	{
	}

	RpgName(const char* cstr) noexcept
		: CharArray()
	{
		CopyFromCstr(cstr);
	}

public:
	inline RpgName& operator=(const char* rhs) noexcept
	{
		if (CharArray != rhs)
		{
			CopyFromCstr(rhs);
		}

		return *this;
	}

	inline char* operator*() noexcept
	{
		return CharArray;
	}

	inline const char* operator*() const noexcept
	{
		return CharArray;
	}

	inline bool operator==(const RpgName& rhs) const noexcept
	{
		return RpgPlatformMemory::CStringCompare(CharArray, rhs.CharArray, true);
	}

	inline bool operator==(const char* rhs) const noexcept
	{
		return RpgPlatformMemory::CStringCompare(CharArray, rhs, true);
	}

	inline bool operator!=(const RpgName& rhs) const noexcept
	{
		return !RpgPlatformMemory::CStringCompare(CharArray, rhs.CharArray, true);
	}


private:
	inline void CopyFromCstr(const char* cstr) noexcept
	{
		RpgPlatformMemory::MemZero(CharArray, RPG_NAME_MAX_COUNT);

		if (cstr && RpgPlatformMemory::CStringLength(cstr) > 0)
		{
			snprintf(CharArray, RPG_NAME_MAX_COUNT, "%s", cstr);
		}
	}

public:
	[[nodiscard]] inline char* GetData() noexcept
	{
		return CharArray;
	}

	[[nodiscard]] inline const char* GetData() const noexcept
	{
		return CharArray;
	}

	[[nodiscard]] inline int GetLength() const noexcept
	{
		return RpgPlatformMemory::CStringLength(CharArray);
	}

	[[nodiscard]] inline bool IsEmpty() const noexcept
	{
		return GetLength() == 0;
	}

public:
	[[nodiscard]] static inline RpgName Format(const char* format, ...) noexcept
	{
		char buffer[RPG_NAME_MAX_COUNT];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, RPG_NAME_MAX_COUNT, format, args);
		va_end(args);

		return buffer;
	}

};



[[nodiscard]] inline uint64_t Rpg_GetHash(const RpgName& value) noexcept
{
	return RpgPlatformMemory::CStringHash(*value);
}
