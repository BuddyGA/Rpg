#pragma once

#include "core/RpgMesh.h"
#include "RpgMaterial.h"



typedef RpgSharedPtr<class RpgModel> RpgSharedModel;

class RpgModel
{
	RPG_NOCOPY(RpgModel)

private:
	RpgName Name;
	int MeshCount;
	int LodCount;
	RpgBoundingAABB Bound;

	RpgSharedMesh Meshes[RPG_MODEL_MAX_MESH][RPG_MODEL_MAX_LOD];
	RpgSharedMaterial Materials[RPG_MODEL_MAX_MESH];


private:
	RpgModel(const RpgName& name) noexcept;

public:
	~RpgModel() noexcept;

	int AddMeshEmpty() noexcept;
	int AddMesh(const RpgSharedMesh& mesh) noexcept;
	void UpdateBound() noexcept;


	inline int AddLod() noexcept
	{
		const int lodIndex = LodCount;
		RPG_PLATFORM_Check(lodIndex >= 0 && lodIndex < RPG_MODEL_MAX_LOD);
		++LodCount;

		return lodIndex;
	}


	inline void SetMaterial(int meshIndex, const RpgSharedMaterial& material) noexcept
	{
		RPG_PLATFORM_Check(meshIndex >= 0 && meshIndex < MeshCount);
		Materials[meshIndex] = material;
	}


	[[nodiscard]] inline const RpgName& GetName() const noexcept
	{
		return Name;
	}

	[[nodiscard]] inline int GetLodCount() const noexcept
	{
		return LodCount;
	}

	[[nodiscard]] inline int GetMeshCount() const noexcept
	{
		return MeshCount;
	}

	[[nodiscard]] inline bool HasSkin() const noexcept
	{
		return Meshes[0][0]->HasSkin();
	}

	[[nodiscard]] inline RpgBoundingAABB GetBound() const noexcept
	{
		return Bound;
	}


	[[nodiscard]] inline RpgSharedMesh& GetMeshLod(int meshIndex, int lodIndex) noexcept
	{
		RPG_PLATFORM_Check(meshIndex >= 0 && meshIndex < MeshCount);
		RPG_PLATFORM_Check(lodIndex >= 0 && lodIndex < LodCount);

		return Meshes[meshIndex][lodIndex];
	}

	[[nodiscard]] inline const RpgSharedMesh& GetMeshLod(int meshIndex, int lodIndex) const noexcept
	{
		RPG_PLATFORM_Check(meshIndex >= 0 && meshIndex < MeshCount);
		RPG_PLATFORM_Check(lodIndex >= 0 && lodIndex < LodCount);

		return Meshes[meshIndex][lodIndex];
	}


	[[nodiscard]] inline const RpgSharedMaterial& GetMaterial(int meshIndex) const noexcept
	{
		RPG_PLATFORM_Check(meshIndex >= 0 && meshIndex < MeshCount);

		return Materials[meshIndex];
	}


public:
	[[nodiscard]] static RpgSharedModel s_CreateShared(const RpgName& name) noexcept;

};
