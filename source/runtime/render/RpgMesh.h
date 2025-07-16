#pragma once

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


public:
	RpgMesh(const RpgName& name) noexcept;
	~RpgMesh() noexcept;

	// Free memory and overwrite existing vertex data 
	void UpdateVertexData(int vertexCount, const RpgVertex::FMeshPosition* positionData, const RpgVertex::FMeshNormalTangent* normalTangentData, const RpgVertex::FMeshTexCoord* texCoordData, const RpgVertex::FMeshSkin* skinData, int indexCount, const RpgVertex::FIndex* indexData) noexcept;

	// Add vertex data to combine/merge in this single batch. Modify the index data internally if there's already vertex data
	void AddBatchVertexData(int vertexCount, const RpgVertex::FMeshPosition* positionData, const RpgVertex::FMeshNormalTangent* normalTangentData, const RpgVertex::FMeshTexCoord* texCoordData, const RpgVertex::FMeshSkin* skinData, int indexCount, const RpgVertex::FIndex* indexData) noexcept;

	// Calculate bounding volume AABB
	RpgBoundingAABB CalculateBound() const noexcept;


	// Clear vertex data
	// @param bFreeMemory - Set TRUE to free memory instead of reset the container
	inline void Clear(bool bFreeMemory) noexcept
	{
		WriteLockAll();
		{
			Positions.Clear(bFreeMemory);
			NormalTangents.Clear(bFreeMemory);
			TexCoords.Clear(bFreeMemory);
			Skins.Clear(bFreeMemory);
			Indices.Clear(bFreeMemory);
			Flags = FLAG_None;
		}
		ReadWriteUnlockAll();
	}


	// Reader lock vertex data. Must call VertexReadUnlock after done reading!
	inline FVertexData VertexReadLock() const noexcept
	{
		ReadLockAll();

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
		ReadWriteUnlockAll();
	}


	// Copy vertex position data
	// @param dst - Copy destination
	// @param out_DstOffset - Offset from the start of <dst> pointer. Value will be added the amount of vertex data size bytes if copy process committed
	// @return TRUE if mesh contains vertex position data and copy committed
	inline bool CopyVertexData_Position(void* dst, size_t& out_DstOffset) const noexcept
	{
		return CopyVertexData<RpgVertex::FMeshPosition>(LockPosition, dst, out_DstOffset, Positions.GetData(), Positions.GetCount());
	}

	// Copy vertex normal-tangent data
	// @param dst - Copy destination
	// @param out_DstOffset - Offset from the start of <dst> pointer. Value will be added the amount of vertex data size bytes if copy process committed
	// @return TRUE if mesh contains vertex normal-tangent data and copy committed
	inline bool CopyVertexData_NormalTangent(void* dst, size_t& out_DstOffset) const noexcept
	{
		return CopyVertexData<RpgVertex::FMeshNormalTangent>(LockNormalTangent, dst, out_DstOffset, NormalTangents.GetData(), NormalTangents.GetCount());
	}

	// Copy vertex texcoord data
	// @param dst - Copy destination
	// @param out_DstOffset - Offset from the start of <dst> pointer. Value will be added the amount of vertex data size bytes if copy process committed
	// @return TRUE if mesh contains vertex texcoord data and copy committed
	inline bool CopyVertexData_TexCoord(void* dst, size_t& out_DstOffset) const noexcept
	{
		return CopyVertexData<RpgVertex::FMeshTexCoord>(LockTexCoord, dst, out_DstOffset, TexCoords.GetData(), TexCoords.GetCount());
	}

	// Copy vertex skin data
	// @param dst - Copy destination
	// @param out_DstOffset - Offset from the start of <dst> pointer. Value will be added the amount of vertex data size bytes if copy process committed
	// @return TRUE if mesh contains vertex skin data and copy committed
	inline bool CopyVertexData_Skin(void* dst, size_t& out_DstOffset) const noexcept
	{
		return CopyVertexData<RpgVertex::FMeshSkin>(LockSkin, dst, out_DstOffset, Skins.GetData(), Skins.GetCount());
	}

	// Copy index data
	// @param dst - Copy destination
	// @param out_DstOffset - Offset from the start of <dst> pointer. Value will be added the amount of index data size bytes if copy process committed
	// @return TRUE if mesh contains index data and copy committed
	inline bool CopyIndexData(void* dst, size_t& out_DstOffset) const noexcept
	{
		return CopyVertexData<RpgVertex::FIndex>(LockIndex, dst, out_DstOffset, Indices.GetData(), Indices.GetCount());
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

	// Check if mesh contains vertex position
	inline bool HasPosition() const noexcept
	{
		return (Flags & FLAG_Attribute_Position);
	}

	// Check if mesh contains vertex normal-tangent
	inline bool HasNormalTangent() const noexcept
	{
		return (Flags & FLAG_Attribute_NormalTangent);
	}

	// Check if mesh contains vertex texcoord
	inline bool HasTexCoord() const noexcept
	{
		return (Flags & FLAG_Attribute_TexCoord);
	}

	// Check if mesh contains vertex skin
	inline bool HasSkin() const noexcept
	{
		return (Flags & FLAG_Attribute_Skin);
	}

	// Check if mesh contains vertex index
	inline bool HasIndex() const noexcept
	{
		return (Flags & FLAG_Attribute_Index);
	}


private:
	inline void WriteLockAll() noexcept
	{
		SDL_LockRWLockForWriting(LockPosition);
		SDL_LockRWLockForWriting(LockNormalTangent);
		SDL_LockRWLockForWriting(LockTexCoord);
		SDL_LockRWLockForWriting(LockSkin);
		SDL_LockRWLockForWriting(LockIndex);
	}

	inline void ReadLockAll() const noexcept
	{
		SDL_LockRWLockForReading(LockPosition);
		SDL_LockRWLockForReading(LockNormalTangent);
		SDL_LockRWLockForReading(LockTexCoord);
		SDL_LockRWLockForReading(LockSkin);
		SDL_LockRWLockForReading(LockIndex);
	}

	inline void ReadWriteUnlockAll() const noexcept
	{
		SDL_UnlockRWLock(LockPosition);
		SDL_UnlockRWLock(LockNormalTangent);
		SDL_UnlockRWLock(LockTexCoord);
		SDL_UnlockRWLock(LockSkin);
		SDL_UnlockRWLock(LockIndex);
	}


	template<typename TVertex>
	inline bool CopyVertexData(SDL_RWLock* lock, void* dst, size_t& out_DstOffset, const TVertex* srcData, int srcCount) const noexcept
	{
		RPG_Assert(lock);
		RPG_Assert(dst);

		if (srcData == nullptr || srcCount == 0)
		{
			return false;
		}

		SDL_LockRWLockForReading(lock);
		{
			const size_t sizeBytes = sizeof(TVertex) * srcCount;
			RpgPlatformMemory::MemCopy(reinterpret_cast<uint8_t*>(dst) + out_DstOffset, srcData, sizeBytes);
			out_DstOffset += sizeBytes;
		}
		SDL_UnlockRWLock(lock);

		return true;
	}


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


	// Vertex attribute flags
	enum EFlag : uint8_t
	{
		FLAG_None = (0),
		FLAG_Attribute_Position			= (1 << 0),
		FLAG_Attribute_NormalTangent	= (1 << 1),
		FLAG_Attribute_TexCoord			= (1 << 2),
		FLAG_Attribute_Skin				= (1 << 3),
		FLAG_Attribute_Index			= (1 << 4),
	};
	uint8_t Flags;


	// Reader-writer lock (vertex position)
	SDL_RWLock* LockPosition;

	// Reader-writer lock (vertex normal-tangent)
	SDL_RWLock* LockNormalTangent;

	// Reader-writer lock (vertex texcoord)
	SDL_RWLock* LockTexCoord;

	// Reader-writer lock (vertex skin)
	SDL_RWLock* LockSkin;

	// Reader-writer lock (vertex index)
	SDL_RWLock* LockIndex;


public:
	// Create shared mesh
	// @param name - Mesh name
	// @return Shared pointer of type <RpgMesh>
	[[nodiscard]] static RpgSharedMesh s_CreateShared(const RpgName& name) noexcept;

};
