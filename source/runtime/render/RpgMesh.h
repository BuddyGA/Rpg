#pragma once

#include "core/RpgConfig.h"
#include "core/RpgString.h"
#include "core/RpgPointer.h"
#include "core/RpgVertex.h"



typedef RpgSharedPtr<class RpgMesh> RpgSharedMesh;

class RpgMesh
{
	RPG_NOCOPY(RpgMesh)

public:
	struct FVertexData
	{
		const RpgVertex::FMeshPosition* PositionData{ nullptr };
		size_t PositionSizeBytes{ 0 };

		const RpgVertex::FMeshNormalTangent* NormalTangentData{ nullptr };
		size_t NormalTangentSizeBytes{ 0 };

		const RpgVertex::FMeshTexCoord* TexCoordData{ nullptr };
		size_t TexCoordSizeBytes{ 0 };

		const RpgVertex::FMeshSkin* SkinData{ nullptr };
		size_t SkinSizeBytes{ 0 };

		const RpgVertex::FIndex* IndexData{ nullptr };
		size_t IndexSizeBytes{ 0 };

		int VertexCount{ 0 };
		int IndexCount{ 0 };
	};

private:
	// Name
	RpgName Name;

	// Vertex position data
	RpgVertexMeshPositionArray Positions;

	// Vertex normal, tangent data
	RpgVertexMeshNormalTangentArray NormalTangents;

	// Vertex attribute data
	RpgVertexMeshTexCoordArray TexCoords;

	// Vertex skin data
	RpgVertexMeshSkinArray Skins;

	// Index data
	RpgVertexIndexArray Indices;

	// Reader-writer lock
	SDL_RWLock* Lock;


private:
	RpgMesh(const RpgName& name) noexcept;

public:
	~RpgMesh() noexcept;

	// Free memory and overwrite existing vertex data 
	void UpdateVertexData(int vertexCount, const RpgVertex::FMeshPosition* positionData, const RpgVertex::FMeshNormalTangent* normalTangentData, const RpgVertex::FMeshTexCoord* texCoordData, const RpgVertex::FMeshSkin* skinData, int indexCount, const RpgVertex::FIndex* indexData) noexcept;

	// Add vertex data to combine/merge in this single batch. Modify the index data internally if there's already vertex data
	void AddBatchVertexData(int vertexCount, const RpgVertex::FMeshPosition* positionData, const RpgVertex::FMeshNormalTangent* normalTangentData, const RpgVertex::FMeshTexCoord* texCoordData, const RpgVertex::FMeshSkin* skinData, int indexCount, const RpgVertex::FIndex* indexData) noexcept;

	// Calculate bounding AABB
	RpgBoundingAABB CalculateBounding() const noexcept;


	// Reset vertex data memory. if <bFreeMemory> is TRUE, free memory instead of only reset the container
	inline void Clear(bool bFreeMemory) noexcept
	{
		SDL_LockRWLockForWriting(Lock);
		{
			Positions.Clear(bFreeMemory);
			NormalTangents.Clear(bFreeMemory);
			TexCoords.Clear(bFreeMemory);
			Skins.Clear(bFreeMemory);
			Indices.Clear(bFreeMemory);
		}
		SDL_UnlockRWLock(Lock);
	}


	// Reader lock vertex data. Must call VertexReadUnlock after done reading!
	inline FVertexData VertexReadLock() const noexcept
	{
		SDL_LockRWLockForReading(Lock);

		FVertexData data;
		data.PositionData = Positions.GetData();
		data.PositionSizeBytes = Positions.GetMemorySizeBytes_Allocated();
		data.NormalTangentData = NormalTangents.GetData();
		data.NormalTangentSizeBytes = NormalTangents.GetMemorySizeBytes_Allocated();
		data.TexCoordData = TexCoords.GetData();
		data.TexCoordSizeBytes = TexCoords.GetMemorySizeBytes_Allocated();
		data.SkinData = Skins.GetData();
		data.SkinSizeBytes = Skins.GetMemorySizeBytes_Allocated();
		data.IndexData = Indices.GetData();
		data.IndexSizeBytes = Indices.GetMemorySizeBytes_Allocated();
		data.VertexCount = Positions.GetCount();
		data.IndexCount = Indices.GetCount();

		return data;
	}

	// Reader unlock vertex data. Must call VertexReadLock previously!
	inline void VertexReadUnlock() const noexcept
	{
		SDL_UnlockRWLock(Lock);
	}


private:
	template<typename TVertex>
	inline void CopyVertexData(void* dst, size_t& out_DstOffset, const TVertex* srcData, int srcCount) const noexcept
	{
		SDL_LockRWLockForReading(Lock);
		{
			const size_t sizeBytes = sizeof(TVertex) * srcCount;
			RpgPlatformMemory::MemCopy(reinterpret_cast<uint8_t*>(dst) + out_DstOffset, srcData, sizeBytes);
			out_DstOffset += sizeBytes;
		}
		SDL_UnlockRWLock(Lock);
	}


public:
	inline void CopyVertexData_Position(void* dst, size_t& out_DstOffset) const noexcept
	{
		CopyVertexData<RpgVertex::FMeshPosition>(dst, out_DstOffset, Positions.GetData(), Positions.GetCount());
	}

	inline void CopyVertexData_NormalTangent(void* dst, size_t& out_DstOffset) const noexcept
	{
		CopyVertexData<RpgVertex::FMeshNormalTangent>(dst, out_DstOffset, NormalTangents.GetData(), NormalTangents.GetCount());
	}

	inline void CopyVertexData_TexCoord(void* dst, size_t& out_DstOffset) const noexcept
	{
		CopyVertexData<RpgVertex::FMeshTexCoord>(dst, out_DstOffset, TexCoords.GetData(), TexCoords.GetCount());
	}

	inline void CopyVertexData_Skin(void* dst, size_t& out_DstOffset) const noexcept
	{
		CopyVertexData<RpgVertex::FMeshSkin>(dst, out_DstOffset, Skins.GetData(), Skins.GetCount());
	}

	inline void CopyIndexData(void* dst, size_t& out_DstOffset) const noexcept
	{
		CopyVertexData<RpgVertex::FIndex>(dst, out_DstOffset, Indices.GetData(), Indices.GetCount());
	}


	// Get mesh name
	inline const RpgName& GetName() const noexcept
	{
		return Name;
	}

	// Get vertex count
	inline int GetVertexCount() const noexcept
	{
		return Positions.GetCount();
	}

	// Get index count
	inline int GetIndexCount() const noexcept
	{
		return Indices.GetCount();
	}

	// TRUE if vertex data contains skin (skinned mesh)
	inline bool HasSkin() const noexcept
	{
		return Skins.GetCount() > 0;
	}


public:
	// Create shared mesh
	[[nodiscard]] static RpgSharedMesh s_CreateShared(const RpgName& name) noexcept;

};
