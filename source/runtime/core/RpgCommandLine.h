#pragma once

#include "RpgPlatform.h"



namespace RpgCommandLine
{
	extern void Initialize(const char* commandArgs) noexcept;

	[[nodiscard]] extern bool HasCommand(const char* command) noexcept;
	[[nodiscard]] extern const char* GetCommandValue(const char* command) noexcept;


	[[nodiscard]] inline int GetCommandValueInt(const char* command) noexcept
	{
		return RpgPlatformMemory::CStringToInt(GetCommandValue(command));
	}

};
