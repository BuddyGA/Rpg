#pragma once

#include "core/RpgThreadPool.h"
#include "core/RpgFilePath.h"
#include "render/RpgTexture.h"
#include "RpgAssimpTypes.h"



class RpgAsyncTask_ImportTexture : public RpgThreadTask
{
public:
	RpgFilePath SourceFilePath;
	RpgAssimp::FTextureEmbedded SourceEmbedded;
	RpgTextureFormat::EType Format;
	bool bGenerateMipMaps;

private:
	RpgSharedTexture2D Result;


public:
	RpgAsyncTask_ImportTexture() noexcept;


	[[nodiscard]] inline RpgSharedTexture2D GetResult() noexcept
	{
		return std::move(Result);
	}


	virtual void Reset() noexcept override;
	virtual void Execute() noexcept override;

	virtual const char* GetTaskName() const noexcept override
	{
		return "RpgAsyncTask_ImportTexture";
	}

};
