#include "RpgPlatform.h"
#include <mimalloc-new-delete.h>



// ========================================================================================================================= //
// PLATFORM - MEMORY
// ========================================================================================================================= //
void* RpgPlatformMemory::MemMalloc(size_t sizeBytes) noexcept
{
	return mi_malloc(sizeBytes);
}


void* RpgPlatformMemory::MemMallocAligned(size_t sizeBytes, size_t alignmentBytes) noexcept
{
	return mi_malloc_aligned(sizeBytes, alignmentBytes);
}


void* RpgPlatformMemory::MemRealloc(void* prevAlloc, size_t newSizeBytes) noexcept
{
	return mi_realloc(prevAlloc, newSizeBytes);
}


void* RpgPlatformMemory::MemRecalloc(void* prevAlloc, int count, size_t sizeBytes) noexcept
{
	return mi_recalloc(prevAlloc, count, sizeBytes);
}


void RpgPlatformMemory::MemFree(void* alloc) noexcept
{
	mi_free(alloc);
}


void RpgPlatformMemory::MemCopy(void* dst, const void* src, size_t sizeBytes) noexcept
{
	memcpy(dst, src, sizeBytes);
}


void RpgPlatformMemory::MemMove(void* dst, const void* src, size_t sizeBytes) noexcept
{
	memmove(dst, src, sizeBytes);
}


void RpgPlatformMemory::MemSet(void* data, int value, size_t sizeBytes) noexcept
{
	memset(data, value, sizeBytes);
}


void RpgPlatformMemory::MemZero(void* data, size_t sizeBytes) noexcept
{
	memset(data, 0, sizeBytes);
}


int RpgPlatformMemory::CStringLength(const char* cstr) noexcept
{
	return static_cast<int>(strlen(cstr));
}


bool RpgPlatformMemory::CStringCompare(const char* cstrA, const char* cstrB, bool bIgnoreCase) noexcept
{
	if (cstrA == nullptr && cstrB == nullptr)
	{
		return true;
	}

	if ((cstrA && cstrB == nullptr) || (cstrA == nullptr && cstrB))
	{
		return false;
	}

	const int lenA = CStringLength(cstrA);
	const int lenB = CStringLength(cstrB);

	if (lenA != lenB)
	{
		return false;
	}

	if (bIgnoreCase)
	{
		for (int i = 0; i < lenA; ++i)
		{
			if (tolower(cstrA[i]) != tolower(cstrB[i]))
			{
				return false;
			}
		}
	}

	return strcmp(cstrA, cstrB) == 0;
}


void RpgPlatformMemory::CStringCopy(char* dst, const char* src) noexcept
{
	strcpy(dst, src);
}


void RpgPlatformMemory::CStringToWide(wchar_t* dst, const char* src, size_t maxBufferCount) noexcept
{
	mbstowcs(dst, src, maxBufferCount);
}


void RpgPlatformMemory::CStringToLower(char* cstr, int len) noexcept
{
	RPG_Assert(cstr);
	RPG_Assert(len > 0);

	for (int i = 0; i < len; ++i)
	{
		cstr[i] = static_cast<char>(tolower(cstr[i]));
	}
}


int RpgPlatformMemory::CStringToInt(const char* cstr) noexcept
{
	return cstr ? atoi(cstr) : 0;
}


uint64_t RpgPlatformMemory::CStringHash(const char* cstr) noexcept
{
	const int len = CStringLength(cstr);
	if (len == 0)
	{
		return 0;
	}

	uint64_t hash = 0;

	for (int i = 0; i < len; ++i)
	{
		hash += cstr[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}




// ========================================================================================================================= //
// PLATFORM - CONSOLE
// ========================================================================================================================= //
#define RPG_WINDOWS_CONSOLE_DEFAULT_ATTRIBUTES		(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)


namespace RpgPlatformConsole
{
	static bool bInitialized = false;

};


void RpgPlatformConsole::Initialize() noexcept
{
	if (bInitialized)
	{
		return;
	}

	// Check if console already exists
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (consoleHandle == NULL || consoleHandle == INVALID_HANDLE_VALUE)
	{
		if (AllocConsole())
		{
			consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		}
	}

	// Double check console handle
	if (consoleHandle == NULL || consoleHandle == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("ERROR: Initialize console failed!\n");
		return;
	}

	HWND consoleWindow = GetConsoleWindow();
	if (consoleWindow == NULL || consoleWindow == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("ERROR: Fail to get console window!\n");
		return;
	}

	SetWindowPos(consoleWindow, NULL, 16, 16, 1280, 480, SWP_SHOWWINDOW);

	bInitialized = true;
}


void RpgPlatformConsole::OutputMessage(const char* message, int messageLength, EOutputColor color) noexcept
{
	if (!bInitialized || message == nullptr || messageLength == 0)
	{
		return;
	}

	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (consoleHandle == NULL || consoleHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}

	WORD attributes = RPG_WINDOWS_CONSOLE_DEFAULT_ATTRIBUTES;

	switch (color)
	{
		case OUTPUT_COLOR_GREEN: attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
		case OUTPUT_COLOR_YELLOW: attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
		case OUTPUT_COLOR_RED: attributes = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
		default: break;
	}

	SetConsoleTextAttribute(consoleHandle, attributes);
	WriteConsoleA(consoleHandle, message, static_cast<DWORD>(messageLength), NULL, NULL);
	SetConsoleTextAttribute(consoleHandle, RPG_WINDOWS_CONSOLE_DEFAULT_ATTRIBUTES);
}




// ========================================================================================================================= //
// PLATFORM - LOG
// ========================================================================================================================= //
namespace RpgPlatformLog
{
	static EVerbosity GlobalVerbosity;
	static HANDLE OutputFileHandle;
	static CRITICAL_SECTION OutputFileCS;
	static bool bInitialized;
};


void RpgPlatformLog::Initialize(EVerbosity in_GlobalVerbosity, const char* opt_OutputLogFilePath) noexcept
{
	if (bInitialized)
	{
		return;
	}

	GlobalVerbosity = in_GlobalVerbosity;

	if (opt_OutputLogFilePath)
	{
		OutputFileHandle = CreateFileA(opt_OutputLogFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		InitializeCriticalSection(&OutputFileCS);
	}

	bInitialized = true;
}


void RpgPlatformLog::Shutdown() noexcept
{
	if (!bInitialized)
	{
		return;
	}

	if (OutputFileHandle)
	{
		CloseHandle(OutputFileHandle);
		OutputFileHandle = NULL;

		DeleteCriticalSection(&OutputFileCS);
	}

	bInitialized = false;
}


void RpgPlatformLog::SetGlobalVerbosity(EVerbosity in_Verbosity) noexcept
{
	GlobalVerbosity = in_Verbosity;
}


void RpgPlatformLog::OutputMessage(RpgPlatformConsole::EOutputColor consoleOutputColor, const char* message) noexcept
{
	const int len = static_cast<int>(SDL_strlen(message));
	if (len == 0)
	{
		return;
	}

	OutputDebugString(message);

	EnterCriticalSection(&OutputFileCS);

	RpgPlatformConsole::OutputMessage(message, len, consoleOutputColor);

	if (OutputFileHandle && OutputFileHandle != INVALID_HANDLE_VALUE)
	{
		WriteFile(OutputFileHandle, message, len, NULL, NULL);
	}

	LeaveCriticalSection(&OutputFileCS);
}


void RpgPlatformLog::OutputMessageFormat(RpgPlatformConsole::EOutputColor consoleOutputColor, const char* format, ...) noexcept
{
	char message[RPG_LOG_MAX_OUTPUT_BUFFER]{};
	
	va_list args;
	va_start(args, format);
	vsnprintf(message, RPG_LOG_MAX_OUTPUT_BUFFER, format, args);
	va_end(args);

	OutputMessage(consoleOutputColor, message);
}


void RpgPlatformLog::OutputMessageLogCategoryFormat(const FCategory& category, EVerbosity verbosity, const char* format, ...) noexcept
{
	if (verbosity < category.Verbosity || category.Verbosity < GlobalVerbosity)
	{
		return;
	}

	char message[RPG_LOG_CATEGORY_MAX_FORMAT]{};

	va_list args;
	va_start(args, format);
	vsnprintf(message, RPG_LOG_CATEGORY_MAX_FORMAT, format, args);
	va_end(args);

	RpgPlatformConsole::EOutputColor consoleOutputColor = RpgPlatformConsole::OUTPUT_COLOR_DEFAULT;

	switch (verbosity)
	{
		case VERBOSITY_DEBUG:
		{
			consoleOutputColor = RpgPlatformConsole::OUTPUT_COLOR_GREEN;
			break;
		}

		case VERBOSITY_LOG:
		{
			consoleOutputColor = RpgPlatformConsole::OUTPUT_COLOR_DEFAULT;
			break;
		}

		case VERBOSITY_WARN:
		{
			consoleOutputColor = RpgPlatformConsole::OUTPUT_COLOR_YELLOW;
			break;
		}

		case VERBOSITY_ERROR:
		{
			consoleOutputColor = RpgPlatformConsole::OUTPUT_COLOR_RED;
			break;
		}

		default:
			break;
	}

	OutputMessageFormat(consoleOutputColor, "<%s>: %s\n", category.Name, message);
}


RPG_LOG_DEFINE_CATEGORY(RpgLogTemp, VERBOSITY_DEBUG)
RPG_LOG_DEFINE_CATEGORY(RpgLogSystem, VERBOSITY_LOG)





// ========================================================================================================================= //
// PLATFORM - PROCESS
// ========================================================================================================================= //
static void Rpg_MimallocOutput(const char* msg, void* arg)
{
	RpgPlatformLog::OutputMessage(RpgPlatformConsole::OUTPUT_COLOR_GREEN, msg);
}



namespace RpgPlatformProcess
{
	static SDL_ThreadID MainThreadId;
	static HWND MainWindowHandle;
	static bool bInitialized;
};


void RpgPlatformProcess::Initialize() noexcept
{
	if (bInitialized)
	{
		return;
	}

	RPG_Log(RpgLogSystem, "Initialize platform process [WINDOWS]");

	mi_option_enable(mi_option_allow_large_os_pages);

#ifndef RPG_BUILD_SHIPPING
	mi_register_output(Rpg_MimallocOutput, nullptr);
	mi_version();
	mi_stats_reset();
#endif // !RPG_BUILD_SHIPPING

	RPG_RuntimeErrorCheck(SDL_Init(SDL_INIT_VIDEO), "SDL initialization failed!");

	MainThreadId = SDL_GetCurrentThreadID();

	bInitialized = true;
}


void RpgPlatformProcess::Shutdown() noexcept
{
	if (!bInitialized)
	{
		return;
	}

	RPG_Log(RpgLogSystem, "Shutdown platform process [WINDOWS]");

	SDL_Quit();

	bInitialized = false;
}


void RpgPlatformProcess::Exit(uint32_t code) noexcept
{
	ExitProcess(code);
}


void RpgPlatformProcess::ShowMessageBoxError(const char* title, const char* message) noexcept
{
	MessageBoxA(IsMainThread() ? MainWindowHandle : NULL, message, title, MB_ICONERROR | MB_OK);
}


void RpgPlatformProcess::SetMainWindowHandle(HWND handle) noexcept
{
	RPG_Assert(handle);
	MainWindowHandle = handle;
}


HWND RpgPlatformProcess::GetMainWindowHandle() noexcept
{
	return MainWindowHandle;
}


uint32_t RpgPlatformProcess::GetMainThreadId() noexcept
{
	return static_cast<uint32_t>(MainThreadId);
}


bool RpgPlatformProcess::IsMainThread() noexcept
{
	return SDL_GetCurrentThreadID() == MainThreadId;
}




// ========================================================================================================================= //
// PLATFORM - FILE
// ========================================================================================================================= //
bool RpgPlatformFile::File_Write(const char* filePath, const void* data, size_t sizeBytes) noexcept
{
	SDL_IOStream* ctx = SDL_IOFromFile(filePath, "w");
	if (ctx == nullptr)
	{
		RPG_LogError(RpgLogSystem, "Write data to file (%s) failed. Cannot open file!", filePath);
		return false;
	}

	if (data == nullptr || sizeBytes == 0)
	{
		RPG_LogError(RpgLogSystem, "Write data to file (%s) failed. Invalid data or sizeBytes!", filePath);
		return false;
	}

	SDL_WriteIO(ctx, data, sizeBytes);
	SDL_CloseIO(ctx);

	return true;
}


bool RpgPlatformFile::File_Delete(const char* filePath) noexcept
{
	return SDL_RemovePath(filePath);
}
