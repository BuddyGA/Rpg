#include "RpgRenderer2D.h"
#include "RpgRenderPipeline.h"
#include "RpgRenderResource.h"



RpgRenderer2D::RpgRenderer2D() noexcept
{
}


RpgRenderer2D::~RpgRenderer2D() noexcept
{
}


int RpgRenderer2D::GetOrAddMaterialInstanceId(const RpgSharedMaterial& material, const RpgSharedTexture2D& texture, bool bIsText) noexcept
{
	RPG_Assert(material);
	RPG_Assert(texture);

	RpgWeakPtr<RpgMaterial> materialToCheck = material->IsInstance() ? material->GetParentMaterial() : material;
	RpgWeakPtr<RpgTexture2D> textureToCheck = texture ? texture : DefaultTexture;
	int materialInstanceIndex = RPG_INDEX_INVALID;

	for (int i = 0; i < MaterialInstanceTextures.GetCount(); ++i)
	{
		const FMaterialInstanceTexture& check = MaterialInstanceTextures[i];

		if ((check.Material == materialToCheck || check.Material->GetParentMaterial() == materialToCheck) && check.Texture == textureToCheck)
		{
			materialInstanceIndex = i;
			break;
		}
	}

	if (materialInstanceIndex == RPG_INDEX_INVALID)
	{
		materialInstanceIndex = MaterialInstanceTextures.GetCount();

		FMaterialInstanceTexture& instance = MaterialInstanceTextures.Add();
		instance.Material = RpgMaterial::s_CreateSharedInstance(RpgName::Format("%s_inst_%i", *material->GetName(), material.GetRefCount()), material);
		instance.Texture = textureToCheck.AsShared();
		instance.Material->SetParameterTextureValue(bIsText ? RpgMaterialParameterTexture::OPACITY_MASK : RpgMaterialParameterTexture::BASE_COLOR, instance.Texture);
	}

	return materialInstanceIndex;
}


void RpgRenderer2D::Begin(int frameIndex, RpgPointInt viewportDimension) noexcept
{
	if (!DefaultMaterialMesh)
	{
		DefaultMaterialMesh = RpgMaterial::s_GetDefault(RpgMaterialDefault::GUI);
		DefaultTexture = RpgTexture2D::s_GetDefault_White();

		GetOrAddMaterialInstanceId(DefaultMaterialMesh, DefaultTexture, false);
	}

	if (!DefaultMaterialFont)
	{
		DefaultMaterialFont = RpgMaterial::s_GetDefault(RpgMaterialDefault::FONT);
		DefaultFont = RpgFont::s_GetDefault_Roboto();
		GetOrAddMaterialInstanceId(DefaultMaterialFont, DefaultFont->GetTexture(), true);
	}

	ViewportDimension = viewportDimension;

	Clips.Clear();
	CurrentClipIndex = RPG_INDEX_INVALID;

	MeshVertexData.Clear();
	MeshIndexData.Clear();
	LineVertexData.Clear();
	LineIndexData.Clear();

	FFrameData& frame = FrameDatas[frameIndex];
	frame.BatchMeshVertexData.Clear();
	frame.BatchMeshIndexData.Clear();
	frame.BatchDrawClipMeshes.Clear();
	
	PushClipRect(RpgRectInt(0, 0, ViewportDimension.X, ViewportDimension.Y));
}


void RpgRenderer2D::End(int frameIndex) noexcept
{
	PopClipRect();
	RPG_Check(CurrentClipIndex == RPG_INDEX_INVALID);
}


void RpgRenderer2D::AddRect(RpgRectInt rect, RpgColorRGBA color, const RpgSharedTexture2D& texture, const RpgSharedMaterial& material) noexcept
{
	if (rect.GetWidth() == 0 || rect.GetHeight() == 0)
	{
		return;
	}

	FClip& clip = Clips.GetAtLast();

	if (!clip.Rect.TestIntersectRect(rect))
	{
		return;
	}

	FMesh& mesh = clip.Shapes.Add();
	
	mesh.MaterialInstanceIndex = GetOrAddMaterialInstanceId(
		material ? material : DefaultMaterialMesh,
		texture ? texture : DefaultTexture,
		false
	);

	mesh.DataVertexStart = MeshVertexData.GetCount();
	mesh.DataVertexCount = 4;
	mesh.DataIndexStart = MeshIndexData.GetCount();
	mesh.DataIndexCount = 6;

	const RpgVertex::FMesh2D vertices[4] =
	{
		{ DirectX::XMFLOAT2(static_cast<float>(rect.Left),  static_cast<float>(rect.Top)), DirectX::XMFLOAT2(0.0f, 0.0f), color },
		{ DirectX::XMFLOAT2(static_cast<float>(rect.Right), static_cast<float>(rect.Top)), DirectX::XMFLOAT2(1.0f, 0.0f), color },
		{ DirectX::XMFLOAT2(static_cast<float>(rect.Right), static_cast<float>(rect.Bottom)), DirectX::XMFLOAT2(1.0f, 1.0f), color },
		{ DirectX::XMFLOAT2(static_cast<float>(rect.Left),  static_cast<float>(rect.Bottom)), DirectX::XMFLOAT2(0.0f, 1.0f), color },
	};

	MeshVertexData.InsertAtRange(vertices, 4, RPG_INDEX_LAST);

	const RpgVertex::FIndex indices[6] =
	{
		0, 1, 2,
		2, 3, 0
	};

	MeshIndexData.InsertAtRange(indices, 6, RPG_INDEX_LAST);
}


void RpgRenderer2D::AddText(const char* text, int length, int x, int y, RpgColorRGBA color, const RpgSharedFont& font, const RpgSharedMaterial& material) noexcept
{
	if (text == nullptr || length == 0)
	{
		return;
	}

	const RpgFont* useFont = font ? font.Get() : DefaultFont.Get();
	const RpgPointInt textDim = useFont->CalculateTextDimension(text, length);
	RPG_Assert(textDim.X > 0 && textDim.Y > 0);
	const RpgRectInt textBound(x, y, x + textDim.X, y + textDim.Y);

	FClip& clip = Clips.GetAtLast();

	if (!clip.Rect.TestIntersectRect(textBound))
	{
		return;
	}

	FMesh& mesh = clip.Texts.Add();

	mesh.MaterialInstanceIndex = GetOrAddMaterialInstanceId(
		material ? material : DefaultMaterialFont,
		font ? font->GetTexture() : DefaultFont->GetTexture(),
		true
	);

	mesh.DataVertexStart = MeshVertexData.GetCount();
	mesh.DataIndexStart = MeshIndexData.GetCount();
	useFont->GenerateTextVertex(text, length, x, y, color, MeshVertexData, MeshIndexData, &mesh.DataVertexCount, &mesh.DataIndexCount);
}


void RpgRenderer2D::PreRender(RpgRenderFrameContext& frameContext) noexcept
{
	auto LocalFunc_UpdateMeshBatchDraw = [&](FFrameData& frame, const FMesh& mesh, FDrawBatchArray& batchDraws)
	{
		const RpgSharedMaterial& material = MaterialInstanceTextures[mesh.MaterialInstanceIndex].Material;
		const int shaderMaterialId = frameContext.MaterialResource->AddMaterial(material);

		int batchDrawIndex = RPG_INDEX_INVALID;
		if (batchDraws.AddUnique(FDrawBatch(shaderMaterialId), &batchDrawIndex))
		{
			FDrawBatch& batchDraw = batchDraws[batchDrawIndex];
			batchDraw.IndexStart = frame.BatchMeshIndexData.GetCount();
			batchDraw.IndexCount = 0;
		}

		FDrawBatch& batchDraw = batchDraws[batchDrawIndex];
		RPG_Assert(batchDraw.ShaderMaterialId == shaderMaterialId);

		const uint32_t vertexOffset = frame.BatchMeshVertexData.GetCount();
		if (vertexOffset > 0)
		{
			RpgVertexGeometryFactory::UpdateBatchIndices(MeshIndexData, vertexOffset, mesh.DataIndexStart, mesh.DataIndexCount);
		}

		batchDraw.IndexCount += mesh.DataIndexCount;

		frame.BatchMeshVertexData.InsertAtRange(MeshVertexData.GetData(mesh.DataVertexStart), mesh.DataVertexCount, RPG_INDEX_LAST);
		frame.BatchMeshIndexData.InsertAtRange(MeshIndexData.GetData(mesh.DataIndexStart), mesh.DataIndexCount, RPG_INDEX_LAST);
	};


	FFrameData& frame = FrameDatas[frameContext.Index];
	auto& batchClipDrawMeshes = frame.BatchDrawClipMeshes;

	for (int c = 0; c < Clips.GetCount(); ++c)
	{
		const FClip& clip = Clips[c];
		if (clip.Shapes.GetCount() == 0 && clip.Texts.GetCount() == 0)
		{
			continue;
		}

		int batchClipIndex = RPG_INDEX_INVALID;
		if (batchClipDrawMeshes.AddUnique(FDrawBatchClip(clip.Rect), &batchClipIndex))
		{
			FDrawBatchClip& draw = batchClipDrawMeshes[batchClipIndex];
			draw.Rect = clip.Rect;
		}

		FDrawBatchClip& batchClip = batchClipDrawMeshes[batchClipIndex];
		RPG_Assert(batchClip.Rect == clip.Rect);

		for (int s = 0; s < clip.Shapes.GetCount(); ++s)
		{
			LocalFunc_UpdateMeshBatchDraw(frame, clip.Shapes[s], batchClip.BatchDraws);
		}

		for (int t = 0; t < clip.Texts.GetCount(); ++t)
		{
			LocalFunc_UpdateMeshBatchDraw(frame, clip.Texts[t], batchClip.BatchDraws);
		}
	}

	if (!frame.BatchDrawClipMeshes.IsEmpty())
	{
		RpgD3D12::ResizeBuffer(frame.MeshVertexBuffer, frame.BatchMeshVertexData.GetMemorySizeBytes_Allocated(), false);
		RPG_D3D12_SetDebugNameAllocation(frame.MeshVertexBuffer, "RES_R2D_MeshVertexBuffer");

		RpgD3D12::ResizeBuffer(frame.MeshIndexBuffer, frame.BatchMeshIndexData.GetMemorySizeBytes_Allocated(), false);
		RPG_D3D12_SetDebugNameAllocation(frame.MeshIndexBuffer, "RES_R2D_MeshIndexBuffer");
	}
}


void RpgRenderer2D::CommandCopy(const RpgRenderFrameContext& frameContext, ID3D12GraphicsCommandList* cmdList) noexcept
{
	FFrameData& frame = FrameDatas[frameContext.Index];

	if (frame.BatchDrawClipMeshes.IsEmpty())
	{
		return;
	}

	const size_t vtxSizeBytes = frame.BatchMeshVertexData.GetMemorySizeBytes_Allocated();
	const size_t idxSizeBytes = frame.BatchMeshIndexData.GetMemorySizeBytes_Allocated();
	const size_t stagingSizeBytes = vtxSizeBytes + idxSizeBytes;

	RpgD3D12::ResizeBuffer(frame.StagingBuffer, stagingSizeBytes, true);
	RPG_D3D12_SetDebugNameAllocation(frame.StagingBuffer, "STG_Mesh2D");

	uint8_t* stagingMap = RpgD3D12::MapBuffer<uint8_t>(frame.StagingBuffer.Get());
	{
		size_t stagingOffset = 0;
		ID3D12Resource* stagingResource = frame.StagingBuffer->GetResource();

		RpgPlatformMemory::MemCopy(stagingMap + stagingOffset, frame.BatchMeshVertexData.GetData(), vtxSizeBytes);
		cmdList->CopyBufferRegion(frame.MeshVertexBuffer->GetResource(), 0, stagingResource, stagingOffset, vtxSizeBytes);
		stagingOffset += vtxSizeBytes;

		RpgPlatformMemory::MemCopy(stagingMap + stagingOffset, frame.BatchMeshIndexData.GetData(), idxSizeBytes);
		cmdList->CopyBufferRegion(frame.MeshIndexBuffer->GetResource(), 0, stagingResource, stagingOffset, idxSizeBytes);
		stagingOffset += idxSizeBytes;
	}
	RpgD3D12::UnmapBuffer(frame.StagingBuffer.Get());
}


void RpgRenderer2D::CommandDraw(const RpgRenderFrameContext& frameContext, ID3D12GraphicsCommandList* cmdList) noexcept
{
	const FFrameData& frame = FrameDatas[frameContext.Index];

	if (frame.BatchDrawClipMeshes.IsEmpty())
	{
		return;
	}

	// Bind vertex buffer
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = frame.MeshVertexBuffer->GetResource()->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(RpgVertex::FMesh2D);
	vertexBufferView.SizeInBytes = static_cast<UINT>(frame.BatchMeshVertexData.GetMemorySizeBytes_Allocated());

	cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);

	// Bind index buffer
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	indexBufferView.BufferLocation = frame.MeshIndexBuffer->GetResource()->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = static_cast<UINT>(frame.BatchMeshIndexData.GetMemorySizeBytes_Allocated());

	cmdList->IASetIndexBuffer(&indexBufferView);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	RpgShaderViewportParameter viewportParam{};
	viewportParam.Width = static_cast<float>(ViewportDimension.X);
	viewportParam.Height = static_cast<float>(ViewportDimension.Y);
	cmdList->SetGraphicsRoot32BitConstants(RpgRenderPipeline::GRPI_VIEWPORT_PARAM, sizeof(RpgShaderViewportParameter) / 4, &viewportParam, 0);

	const RpgMaterialResource* materialResource = frameContext.MaterialResource;

	for (int i = 0; i < frame.BatchDrawClipMeshes.GetCount(); ++i)
	{
		const FDrawBatchClip& batchDrawClip = frame.BatchDrawClipMeshes[i];
		const RpgRectInt rect = batchDrawClip.Rect;
		RpgD3D12Command::SetScissor(cmdList, rect.Left, rect.Top, rect.Right, rect.Bottom);

		for (int d = 0; d < batchDrawClip.BatchDraws.GetCount(); ++d)
		{
			const FDrawBatch draw = batchDrawClip.BatchDraws[d];
			materialResource->CommandBindMaterial(cmdList, draw.ShaderMaterialId);

			cmdList->DrawIndexedInstanced(draw.IndexCount, 1, draw.IndexStart, 0, 0);
		}
	}
}

