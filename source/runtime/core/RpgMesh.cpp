#include "RpgMesh.h"



RpgMesh::RpgMesh(const RpgName& name) noexcept
{
	Name = name;
	Lock = SDL_CreateRWLock();
}


RpgMesh::~RpgMesh() noexcept
{
	SDL_DestroyRWLock(Lock);
}


void RpgMesh::UpdateVertexData(int vertexCount, const RpgVertex::FMeshPosition* positionData, const RpgVertex::FMeshNormalTangent* normalTangentData, const RpgVertex::FMeshTexCoord* texCoordData, const RpgVertex::FMeshSkin* skinData, int indexCount, const RpgVertex::FIndex* indexData) noexcept
{
	RPG_PLATFORM_Assert(vertexCount > 0);
	RPG_PLATFORM_Assert(positionData && normalTangentData && texCoordData);
	RPG_PLATFORM_Assert(indexCount > 0);
	RPG_PLATFORM_Assert(indexData);

	SDL_LockRWLockForWriting(Lock);
	{
		Positions.Clear(true);
		Positions.InsertAtRange(positionData, vertexCount, RPG_INDEX_LAST);

		NormalTangents.Clear(true);
		NormalTangents.InsertAtRange(normalTangentData, vertexCount, RPG_INDEX_LAST);

		TexCoords.Clear(true);
		TexCoords.InsertAtRange(texCoordData, vertexCount, RPG_INDEX_LAST);

		Skins.Clear(true);

		if (skinData)
		{
			Skins.InsertAtRange(skinData, vertexCount, RPG_INDEX_LAST);
		}

		Indices.Clear(true);
		Indices.InsertAtRange(indexData, indexCount, RPG_INDEX_LAST);
	}
	SDL_UnlockRWLock(Lock);
}


void RpgMesh::AddBatchVertexData(int vertexCount, const RpgVertex::FMeshPosition* positionData, const RpgVertex::FMeshNormalTangent* normalTangentData, const RpgVertex::FMeshTexCoord* texCoordData, const RpgVertex::FMeshSkin* skinData, int indexCount, const RpgVertex::FIndex* indexData) noexcept
{
	RPG_PLATFORM_Assert(vertexCount > 0);
	RPG_PLATFORM_Assert(positionData && normalTangentData && texCoordData);
	RPG_PLATFORM_Assert(indexCount > 0);
	RPG_PLATFORM_Assert(indexData);

	const uint32_t baseVertex = static_cast<uint32_t>(Positions.GetCount());

	if (baseVertex > 0)
	{
		if (HasSkin())
		{
			RPG_PLATFORM_CheckV(skinData, "Vertex data added to batch must have skin data if original mesh contains skin data!");
		}
		else
		{
			RPG_PLATFORM_CheckV(skinData == nullptr, "Vertex data added to batch must not contain skin data if original mesh does not have skin data!");
		}
	}

	SDL_LockRWLockForWriting(Lock);
	{
		Positions.InsertAtRange(positionData, vertexCount, RPG_INDEX_LAST);
		NormalTangents.InsertAtRange(normalTangentData, vertexCount, RPG_INDEX_LAST);
		TexCoords.InsertAtRange(texCoordData, vertexCount, RPG_INDEX_LAST);

		if (skinData)
		{
			Skins.InsertAtRange(skinData, vertexCount, RPG_INDEX_LAST);
		}

		const int baseIndex = Indices.GetCount();
		Indices.InsertAtRange(indexData, indexCount, RPG_INDEX_LAST);

		if (baseVertex > 0)
		{
			RpgVertexGeometryFactory::UpdateBatchIndices(Indices, baseVertex, baseIndex, indexCount);
		}
	}
	SDL_UnlockRWLock(Lock);
}


RpgBoundingAABB RpgMesh::CalculateBounding() const noexcept
{
	RpgBoundingAABB aabb;

	if (Positions.IsEmpty())
	{
		return aabb;
	}

	aabb.Min = FLT_MAX;
	aabb.Max = -FLT_MAX;
	const int vertexCount = Positions.GetCount();

	for (int v = 0; v < vertexCount; ++v)
	{
		const RpgVector3 vec(Positions[v].ToVector3());
		aabb.Min = RpgVector3::Min(aabb.Min, vec);
		aabb.Max = RpgVector3::Max(aabb.Max, vec);
	}

	return aabb;
}


RpgSharedMesh RpgMesh::s_CreateShared(const RpgName& name) noexcept
{
	return RpgSharedMesh(new RpgMesh(name));
}
