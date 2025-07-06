#pragma once

#include "RpgString.h"



class RpgFilePath
{
private:
	RpgString FullPath;
	RpgStringView DirectoryPath;
	RpgStringView DirectoryName;
	RpgStringView FileName;
	RpgStringView FileExt;


public:
	RpgFilePath() noexcept = default;
	~RpgFilePath() noexcept = default;


	RpgFilePath(const RpgFilePath& other) noexcept
		: FullPath(other.FullPath)
	{
		InitializeInternal();
	}

	RpgFilePath(RpgFilePath&& other) noexcept
		: FullPath(std::move(other.FullPath))
		, DirectoryPath(other.DirectoryPath)
		, DirectoryName(other.DirectoryName)
		, FileName(other.FileName)
		, FileExt(other.FileExt)
	{
		other.DirectoryPath = RpgStringView();
		other.DirectoryName = RpgStringView();
		other.FileName = RpgStringView();
		other.FileExt = RpgStringView();
	}

	RpgFilePath(const RpgString& in_Path) noexcept
		: FullPath(in_Path)
	{
		FullPath.ReplaceInPlace('\\', '/');
		InitializeInternal();
	}

	RpgFilePath(RpgString&& in_Path) noexcept
		: FullPath(std::move(in_Path))
	{
		FullPath.ReplaceInPlace('\\', '/');
		InitializeInternal();
	}


public:
	inline RpgFilePath& operator=(const RpgFilePath& rhs) noexcept
	{
		if (this != &rhs)
		{
			FullPath = rhs.FullPath;
			InitializeInternal();
		}

		return *this;
	}

	inline RpgFilePath& operator=(RpgFilePath&& rhs) noexcept
	{
		if (this != &rhs)
		{
			FullPath = std::move(rhs.FullPath);
			DirectoryPath = rhs.DirectoryPath;
			DirectoryName = rhs.DirectoryName;
			FileName = rhs.FileName;
			FileExt = rhs.FileExt;

			rhs.DirectoryPath = RpgStringView();
			rhs.DirectoryName = RpgStringView();
			rhs.FileName = RpgStringView();
			rhs.FileExt = RpgStringView();
		}

		return *this;
	}

	inline RpgFilePath& operator=(const RpgString& rhs) noexcept
	{
		FullPath = rhs;
		FullPath.ReplaceInPlace('\\', '/');
		InitializeInternal();

		return *this;
	}

	inline RpgFilePath& operator=(RpgString&& rhs) noexcept
	{
		if (&FullPath != &rhs)
		{
			FullPath = std::move(rhs);
			FullPath.ReplaceInPlace('\\', '/');
			InitializeInternal();
		}

		return *this;
	}


	[[nodiscard]] inline RpgFilePath operator+(const RpgString& rhs) const noexcept
	{
		return RpgFilePath(FullPath + rhs);
	}

	[[nodiscard]] inline RpgFilePath operator+(const char* rhs) const noexcept
	{
		return RpgFilePath(FullPath + rhs);
	}


	[[nodiscard]] inline const char* operator*() const noexcept
	{
		return *FullPath;
	}


	[[nodiscard]] inline bool operator==(const RpgFilePath& rhs) const noexcept
	{
		return FullPath.Equals(rhs.FullPath, true);
	}

	[[nodiscard]] inline bool operator!=(const RpgFilePath& rhs) const noexcept
	{
		return !FullPath.Equals(rhs.FullPath, true);
	}


private:
	void InitializeInternal();
	[[nodiscard]] bool IsPathValid() const noexcept;


public:
	[[nodiscard]] RpgFilePath GetParentDirectoryPath() const noexcept;
	[[nodiscard]] RpgString GetDirectoryPath() const noexcept;
	[[nodiscard]] RpgName GetDirectoryName() const noexcept;
	[[nodiscard]] RpgName GetFileName() const noexcept;
	[[nodiscard]] RpgName GetFileExtension() const noexcept;


	inline void Clear(bool bFreeMemory = false) noexcept
	{
		FullPath.Clear(bFreeMemory);
		InitializeInternal();
	}


	[[nodiscard]] inline bool IsDirectoryPath() const noexcept
	{
		return !DirectoryPath.IsEmpty() && !DirectoryName.IsEmpty() && FileName.IsEmpty() && FileExt.IsEmpty();
	}

	[[nodiscard]] inline bool IsFilePath() const noexcept
	{
		return !FileName.IsEmpty();
	}

	[[nodiscard]] inline bool HasFileExtension() const noexcept
	{
		return !FileExt.IsEmpty();
	}

	[[nodiscard]] inline const RpgString& ToString() const noexcept
	{
		return FullPath;
	}

};



namespace RpgFileSystem
{
	extern void Initialize() noexcept;

	[[nodiscard]] extern const RpgString& GetExecutableDirPath() noexcept;
	[[nodiscard]] extern const RpgString& GetUserAppDataLocalDirPath() noexcept;
	[[nodiscard]] extern const RpgString& GetUserTempDirPath() noexcept;
	[[nodiscard]] extern const RpgString& GetProjectDirPath() noexcept;
	[[nodiscard]] extern const RpgString& GetSourceDirPath() noexcept;
	[[nodiscard]] extern const RpgString& GetAssetDirPath() noexcept;
	[[nodiscard]] extern const RpgString& GetAssetRawDirPath() noexcept;

};
