#pragma once

#include "core/RpgThreadPool.h"
#include "core/RpgFilePath.h"
#include "render/RpgModel.h"
#include "animation/RpgAnimationTypes.h"
#include "RpgAssimpTypes.h"




class RpgAsyncTask_ImportModel : public RpgThreadTask
{
public:
	RpgFilePath SourceFilePath;
	float Scale;
	bool bImportMaterialTexture;
	bool bImportSkeleton;
	bool bImportAnimation;
	bool bGenerateTextureMipMaps;
	bool bIgnoreTextureNormals;


private:
	RpgArray<class RpgAsyncTask_ImportTexture*> ImportTextureTasks;
	RpgArray<RpgAssimp::FMaterialPhong> IntermediateMaterialPhongs;
	RpgArray<RpgAssimp::FModel> IntermediateModels;

	RpgArray<RpgSharedModel> ImportedModels;
	RpgArray<RpgSharedAnimationClip> ImportedAnimations;
	RpgSharedAnimationSkeleton ImportedSkeleton;


public:
	RpgAsyncTask_ImportModel() noexcept;

private:
	void ExtractMaterialTextures(const aiScene* assimpScene);
	void ExtractSkeleton(const aiMesh* assimpMesh) noexcept;
	void ExtractMeshesFromNode(const aiScene* assimpScene, const aiNode* assimpNode) noexcept;
	void ExtractAnimations(const aiScene* assimpScene) noexcept;

public:
	[[nodiscard]] inline RpgArray<RpgSharedModel> GetImportedModels() noexcept
	{
		return std::move(ImportedModels);
	}

	[[nodiscard]] inline RpgSharedAnimationSkeleton GetImportedSkeleton() noexcept
	{
		return std::move(ImportedSkeleton);
	}

	[[nodiscard]] inline RpgArray<RpgSharedAnimationClip> GetImportedAnimations() noexcept
	{
		return std::move(ImportedAnimations);
	}


	virtual void Reset() noexcept override;
	virtual void Execute() noexcept override;

	virtual const char* GetTaskName() const noexcept
	{
		return "RpgAsyncTask_ImportModel";
	}

};
