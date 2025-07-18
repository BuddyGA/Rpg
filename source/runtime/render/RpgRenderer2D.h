#pragma once

#include "RpgFont.h"
#include "RpgMaterial.h"



class RpgRenderer2D
{
	RPG_NOCOPY(RpgRenderer2D)

public:
	RpgRenderer2D() noexcept;
	~RpgRenderer2D() noexcept;

private:
	int GetOrAddMaterialInstanceId(const RpgSharedMaterial& material, const RpgSharedTexture2D& texture, bool bIsText) noexcept;

public:
	void Begin(int frameIndex, RpgPointInt viewportDimension) noexcept;
	void End(int frameIndex) noexcept;


	inline RpgPointInt GetViewportDimension() const noexcept
	{
		return ViewportDimension;
	}


	inline void PushClipRect(RpgRectInt rect) noexcept
	{
		const int parentClipIndex = CurrentClipIndex;
		CurrentClipIndex = Clips.GetCount();

		FClip& clip = Clips.Add();
		clip.Rect = rect;
		clip.ParentClipIndex = parentClipIndex;
	}

	inline void PopClipRect() noexcept
	{
		CurrentClipIndex = Clips[CurrentClipIndex].ParentClipIndex;
	}


	void AddRect(RpgRectInt rect, RpgColorRGBA color, const RpgSharedTexture2D& texture = RpgSharedTexture2D(), const RpgSharedMaterial& material = RpgSharedMaterial()) noexcept;
	void AddText(const char* text, int length, int x, int y, RpgColorRGBA color, const RpgSharedFont& font = RpgSharedFont(), const RpgSharedMaterial& material = RpgSharedMaterial()) noexcept;

	void PreRender(RpgRenderFrameContext& frameContext) noexcept;
	void CommandCopy(const RpgRenderFrameContext& frameContext, ID3D12GraphicsCommandList* cmdList) noexcept;
	void CommandDraw(const RpgRenderFrameContext& frameContext, ID3D12GraphicsCommandList* cmdList) noexcept;


private:
	RpgSharedMaterial DefaultMaterialMesh;
	RpgSharedMaterial DefaultMaterialFont;
	RpgSharedTexture2D DefaultTexture;
	RpgSharedFont DefaultFont;
	RpgPointInt ViewportDimension;


	struct FMaterialInstanceTexture
	{
		RpgSharedMaterial Material;
		RpgSharedTexture2D Texture;
	};
	RpgArray<FMaterialInstanceTexture> MaterialInstanceTextures;


	struct FMesh
	{
		int MaterialInstanceIndex{ RPG_INDEX_INVALID };
		int DataVertexStart{ 0 };
		int DataVertexCount{ 0 };
		int DataIndexStart{ 0 };
		int DataIndexCount{ 0 };
	};


	struct FClip
	{
		RpgRectInt Rect;
		int ParentClipIndex{ RPG_INDEX_INVALID };
		RpgArrayInline<FMesh, 16> Shapes;
		RpgArrayInline<FMesh, 8> Texts;
	};
	RpgArray<FClip, 16> Clips;

	int CurrentClipIndex;

	RpgVertexMesh2DArray MeshVertexData;
	RpgVertexIndexArray MeshIndexData;

	RpgVertexPrimitive2DArray LineVertexData;
	RpgVertexIndexArray LineIndexData;


	struct FDrawBatch
	{
		int ShaderMaterialId;
		int IndexStart;
		int IndexCount;


		FDrawBatch(int in_MaterialId = RPG_INDEX_INVALID) noexcept
			: ShaderMaterialId(in_MaterialId)
			, IndexStart(0)
			, IndexCount(0)
		{
		}

		inline bool operator==(const FDrawBatch& rhs) const noexcept
		{
			return ShaderMaterialId == rhs.ShaderMaterialId;
		}
	};
	typedef RpgArrayInline<FDrawBatch, 16> FDrawBatchArray;


	struct FDrawBatchClip
	{
		RpgRectInt Rect;
		FDrawBatchArray BatchDraws;


		FDrawBatchClip(const RpgRectInt& in_Rect = RpgRectInt()) noexcept
			: Rect(in_Rect)
		{
		}

		inline bool operator==(const FDrawBatchClip& rhs) const noexcept
		{
			return Rect == rhs.Rect;
		}
	};


	struct FFrameData
	{
		RpgVertexMesh2DArray BatchMeshVertexData;
		RpgVertexIndexArray BatchMeshIndexData;
		RpgArray<FDrawBatchClip, 8> BatchDrawClipMeshes;

		ComPtr<D3D12MA::Allocation> StagingBuffer;
		ComPtr<D3D12MA::Allocation> MeshVertexBuffer;
		ComPtr<D3D12MA::Allocation> MeshIndexBuffer;
		ComPtr<D3D12MA::Allocation> LineVertexBuffer;
		ComPtr<D3D12MA::Allocation> LineIndexBuffer;
	};
	FFrameData FrameDatas[RPG_FRAME_BUFFERING];

};
