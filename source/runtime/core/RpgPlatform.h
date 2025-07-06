#pragma once

#include "RpgTypes.h"

#include <mimalloc-override.h>

#include <winsdkver.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <sdkddkver.h>

#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#include <SDL3/SDL.h>


#define RPG_DebugBreak()	if (IsDebuggerPresent()) __debugbreak()




// ========================================================================================================================= //
// PLATFORM - MEMORY
// ========================================================================================================================= //
namespace RpgPlatformMemory
{
	[[nodiscard]] extern void* MemMalloc(size_t sizeBytes) noexcept;
	[[nodiscard]] extern void* MemMallocAligned(size_t sizeBytes, size_t alignmentBytes) noexcept;
	[[nodiscard]] extern void* MemRealloc(void* prevAlloc, size_t newSizeBytes) noexcept;
	[[nodiscard]] extern void* MemRecalloc(void* prevAlloc, int count, size_t sizeBytes) noexcept;
	extern void MemFree(void* alloc) noexcept;
	extern void MemCopy(void* dst, const void* src, size_t sizeBytes) noexcept;
	extern void MemMove(void* dst, const void* src, size_t sizeBytes) noexcept;
	extern void MemSet(void* data, int value, size_t sizeBytes) noexcept;
	extern void MemZero(void* data, size_t sizeBytes) noexcept;

	[[nodiscard]] extern int CStringLength(const char* cstr) noexcept;
	[[nodiscard]] extern bool CStringCompare(const char* cstrA, const char* cstrB, bool bIgnoreCase) noexcept;
	extern void CStringCopy(char* dst, const char* src) noexcept;
	extern void CStringToWide(wchar_t* dst, const char* src, size_t maxBufferCount) noexcept;
	extern void CStringToLower(char* cstr, int len) noexcept;
	[[nodiscard]] extern int CStringToInt(const char* cstr) noexcept;
	[[nodiscard]] extern uint64_t CStringHash(const char* cstr) noexcept;

}; // RpgPlatformMemory




// ========================================================================================================================= //
// PLATFORM - CONSOLE
// ========================================================================================================================= //
namespace RpgPlatformConsole
{
	enum EOutputColor : uint8_t
	{
		OUTPUT_COLOR_DEFAULT = 0,
		OUTPUT_COLOR_GREEN,
		OUTPUT_COLOR_YELLOW,
		OUTPUT_COLOR_RED
	};


	// Initialize and allocate console window
	// @returns True on success false otherwise
	extern void Initialize() noexcept;


	// Output message to console
	// @param message - Message to output
	// @param color - Message color
	// @returns None
	extern void OutputMessage(const char* message, int messageLength, EOutputColor color) noexcept;

};




// ========================================================================================================================= //
// PLATFORM - LOG
// ========================================================================================================================= //
#define RPG_PLATFORM_LOG_MAX_OUTPUT_BUFFER_COUNT		(2048)
#define RPG_PLATFORM_LOG_CATEGORY_NAME_LENGTH			(31)	// Includes null terminator
#define RPG_PLATFORM_LOG_CATEGORY_MAX_FORMAT_COUNT		(RPG_PLATFORM_LOG_MAX_OUTPUT_BUFFER_COUNT - RPG_PLATFORM_LOG_CATEGORY_NAME_LENGTH)


namespace RpgPlatformLog
{
	enum EVerbosity : uint8_t
	{
		VERBOSITY_NONE = 0,
		VERBOSITY_DEBUG,
		VERBOSITY_LOG,
		VERBOSITY_WARN,
		VERBOSITY_ERROR
	};


	struct FCategory
	{
		const char* Name{ nullptr };
		EVerbosity Verbosity{ VERBOSITY_NONE };
	};


	extern void Initialize(EVerbosity in_GlobalVerbosity, const char* opt_OutputLogFilePath = nullptr) noexcept;
	extern void Shutdown() noexcept;
	extern void SetGlobalVerbosity(EVerbosity in_Verbosity) noexcept;
	extern void OutputMessage(RpgPlatformConsole::EOutputColor consoleOutputColor, const char* message) noexcept;
	extern void OutputMessageFormat(RpgPlatformConsole::EOutputColor consoleOutputColor, const char* format, ...) noexcept;
	extern void OutputMessageLogCategoryFormat(const FCategory& category, EVerbosity verbosity, const char* format, ...) noexcept;

}; // RpgPlatformLog


#define RPG_PLATFORM_LOG_DECLARE_CATEGORY_EXTERN(catName)	extern RpgPlatformLog::FCategory catName;


#define RPG_PLATFORM_LOG_DECLARE_CATEGORY_STATIC(catName, defaultVerbosity)												\
static RpgPlatformLog::FCategory catName{#catName, RpgPlatformLog::defaultVerbosity};									\
static_assert(sizeof(#catName) <= RPG_PLATFORM_LOG_CATEGORY_NAME_LENGTH, "Exceeds maximum log category name length!");


#define RPG_PLATFORM_LOG_DEFINE_CATEGORY(catName, defaultVerbosity)														\
RpgPlatformLog::FCategory catName{#catName, RpgPlatformLog::defaultVerbosity};											\
static_assert(sizeof(#catName) <= RPG_PLATFORM_LOG_CATEGORY_NAME_LENGTH, "Exceeds maximum log category name length!");


#define RPG_PLATFORM_Log(category, format, ...)			RpgPlatformLog::OutputMessageLogCategoryFormat(category, RpgPlatformLog::VERBOSITY_LOG, format, __VA_ARGS__)
#define RPG_PLATFORM_LogDebug(category, format, ...)	RpgPlatformLog::OutputMessageLogCategoryFormat(category, RpgPlatformLog::VERBOSITY_DEBUG, format, __VA_ARGS__)
#define RPG_PLATFORM_LogWarn(category, format, ...)		RpgPlatformLog::OutputMessageLogCategoryFormat(category, RpgPlatformLog::VERBOSITY_WARN, format, __VA_ARGS__)
#define RPG_PLATFORM_LogError(category, format, ...)	RpgPlatformLog::OutputMessageLogCategoryFormat(category, RpgPlatformLog::VERBOSITY_ERROR, format, __VA_ARGS__)


RPG_PLATFORM_LOG_DECLARE_CATEGORY_EXTERN(RpgLogTemp)
RPG_PLATFORM_LOG_DECLARE_CATEGORY_EXTERN(RpgLogSystem)




// ========================================================================================================================= //
// PLATFORM - PROCESS
// ========================================================================================================================= //
namespace RpgPlatformProcess
{
	extern void Initialize() noexcept;
	extern void Shutdown() noexcept;
	extern void Exit(uint32_t code) noexcept;
	extern void ShowMessageBoxError(const char* title, const char* message) noexcept;
	extern void SetMainWindowHandle(HWND handle) noexcept;
	[[nodiscard]] extern HWND GetMainWindowHandle() noexcept;
	[[nodiscard]] extern uint32_t GetMainThreadId() noexcept;


	// Check if function running on main thread
	// @returns True if running on main thread
	[[nodiscard]] extern bool IsMainThread() noexcept;

}; // RpgPlatformProcess




// ========================================================================================================================= //
// PLATFORM - ASSERT
// ========================================================================================================================= //
#ifdef RPG_BUILD_DEBUG
#define RPG_PLATFORM_ASSERT_LEVEL	(0)
#elif RPG_BUILD_DEVELOPMENT
#define RPG_PLATFORM_ASSERT_LEVEL	(1)
#else
#define RPG_PLATFORM_ASSERT_LEVEL	(2)
#endif // RPG_BUILD_DEBUG


#define RPG_PLATFORM_ASSERT_MESSAGE_EXIT(message)								\
RpgPlatformLog::OutputMessage(RpgPlatformConsole::OUTPUT_COLOR_RED, message);	\
RpgPlatformProcess::ShowMessageBoxError("ERROR (333)", message);				\
RPG_DebugBreak();																\
RpgPlatformProcess::Exit(333);		


#define RPG_PLATFORM_AssertMessageV(cond, format, ...)																				\
if (!(cond))																														\
{																																	\
	char message[512];																												\
	snprintf(message, 512, format, __VA_ARGS__);																					\
	char assertMessage[512];																										\
	snprintf(assertMessage, 512, "AssertionFailed: (%s)\nMessage: %s\nFile: %s\nLine: %i\n", #cond, message, __FILE__, __LINE__);	\
	RPG_PLATFORM_ASSERT_MESSAGE_EXIT(assertMessage);																				\
}


#define RPG_PLATFORM_AssertMessage(cond)																		\
if (!(cond))																									\
{																												\
	char assertMessage[512];																					\
	snprintf(assertMessage, 512, "AssertionFailed: (%s)\nFile: %s\nLine: %i\n", #cond, __FILE__, __LINE__);		\
	RPG_PLATFORM_ASSERT_MESSAGE_EXIT(assertMessage);															\
}


#if RPG_PLATFORM_ASSERT_LEVEL < 1
#define RPG_PLATFORM_AssertV(cond, format, ...)	RPG_PLATFORM_AssertMessageV(cond, format, __VA_ARGS__)
#define RPG_PLATFORM_Assert(cond)				RPG_PLATFORM_AssertMessage(cond)
#else
#define RPG_PLATFORM_AssertV(cond, format, ...)
#define RPG_PLATFORM_Assert(cond)
#endif // RPG_PLATFORM_ASSERT_LEVEL < 1


#if RPG_PLATFORM_ASSERT_LEVEL < 2
#define RPG_PLATFORM_CheckV(cond, format, ...)	RPG_PLATFORM_AssertMessageV(cond, format, __VA_ARGS__)
#define RPG_PLATFORM_Check(cond)				RPG_PLATFORM_AssertMessage(cond)
#else
#define RPG_PLATFORM_CheckV(cond, format, ...)
#define RPG_PLATFORM_Check(cond)
#endif // RPG_PLATFORM_ASSERT_LEVEL < 2


#define RPG_PLATFORM_ValidateV(cond, format, ...)	RPG_PLATFORM_AssertMessageV(cond, format, __VA_ARGS__)
#define RPG_PLATFORM_Validate(cond)					RPG_PLATFORM_AssertMessage(cond)


#define RPG_RuntimeErrorCheck(cond, message)															\
if (!(cond))																							\
{																										\
	RpgPlatformLog::OutputMessageFormat(RpgPlatformConsole::OUTPUT_COLOR_RED, "ERROR: %s\n", message);	\
	RpgPlatformProcess::ShowMessageBoxError("Runtime Error", message);									\
	RPG_DebugBreak();																					\
	RpgPlatformProcess::Exit(111);																		\
}


#define RPG_NotImplementedYet()		RPG_RuntimeErrorCheck(false, "Function or scope not implemented yet!");

#define RPG_IsMainThread()			RPG_PLATFORM_Check(RpgPlatformProcess::IsMainThread())




// ========================================================================================================================= //
// PLATFORM - FILE
// ========================================================================================================================= //
namespace RpgPlatformFile
{
	extern bool File_Write(const char* filePath, const void* data, size_t sizeBytes) noexcept;
	extern bool File_Delete(const char* filePath) noexcept;

};
