#include "RpgFont.h"
#include "core/RpgFilePath.h"
#include "thirdparty/stb/stb_truetype.h"


RPG_PLATFORM_LOG_DECLARE_CATEGORY_STATIC(RpgLogFont, VERBOSITY_DEBUG)


RpgFont::RpgFont(const RpgName& in_Name, const RpgString& ttfFilePath, int in_SizePx, int in_UnicodeStart, int in_UnicodeCount) noexcept
{
	RPG_PLATFORM_LogDebug(RpgLogFont, "Create font (%s)", *in_Name);

	Name = in_Name;

	SDL_IOStream* fileCtx = SDL_IOFromFile(*ttfFilePath, "r");
	RPG_PLATFORM_Check(fileCtx);

	int64_t fileSizeBytes = SDL_GetIOSize(fileCtx);
	RPG_PLATFORM_Check(fileSizeBytes > 0);

	RpgArray<uint8_t> fileData(static_cast<int>(fileSizeBytes));
	SDL_ReadIO(fileCtx, fileData.GetData(), fileSizeBytes);

	SDL_CloseIO(fileCtx);

	stbtt_fontinfo stbFontInfo{};
	const int stbError = stbtt_InitFont(&stbFontInfo, fileData.GetData(), 0);
	RPG_PLATFORM_Check(stbError != 0);

	// Init font datas
	RpgPointInt textureDimension(256);
	RpgArray<uint8_t> texturePixels(textureDimension.X * textureDimension.Y);

	Unicode.CodeStart = in_UnicodeStart;
	Unicode.CodeCount = in_UnicodeCount;
	Unicode.PackedChars = RpgPlatformMemory::MemMalloc(sizeof(stbtt_packedchar) * Unicode.CodeCount);

	const float scaleFactor = stbtt_ScaleForPixelHeight(&stbFontInfo, static_cast<float>(in_SizePx));

	stbtt_pack_context stbPackContext{};
	{
		int res = stbtt_PackBegin(&stbPackContext, texturePixels.GetData(), textureDimension.X, textureDimension.Y, 0, 1, nullptr);
		RPG_PLATFORM_Check(res == 1);

		// TODO: Resize texture dimension if packing failed (res == 0)

		stbtt_PackSetOversampling(&stbPackContext, 2, 2);

		res = stbtt_PackFontRange(&stbPackContext, fileData.GetData(), 0, static_cast<float>(in_SizePx), Unicode.CodeStart, Unicode.CodeCount, static_cast<stbtt_packedchar*>(Unicode.PackedChars));
		RPG_PLATFORM_Check(res == 1);

		stbtt_PackEnd(&stbPackContext);
	}

	int tempAscent, tempDescent, tempLineGap;
	stbtt_GetFontVMetrics(&stbFontInfo, &tempAscent, &tempDescent, &tempLineGap);

	Metric.SizePx = in_SizePx;
	Metric.Ascent = static_cast<int>(tempAscent * scaleFactor);
	Metric.Descent = static_cast<int>(tempDescent * scaleFactor);
	Metric.LineSpace = (Metric.Ascent - Metric.Descent) + static_cast<int>(tempLineGap * scaleFactor);

	Texture = RpgTexture2D::s_CreateShared2D(RpgName::Format("TEX2D_%s", *Name), RpgTextureFormat::TEX_2D_R, textureDimension.X, textureDimension.Y, 1);
	{
		RpgTexture2D::FMipData mipData;
		uint8_t* pixelData = Texture->MipWriteLock(0, mipData);
		RpgPlatformMemory::MemCopy(pixelData, texturePixels.GetData(), texturePixels.GetMemorySizeBytes_Allocated());
		Texture->MipReadWriteUnlock(0);
	}
}


RpgFont::~RpgFont() noexcept
{
	RPG_PLATFORM_LogDebug(RpgLogFont, "Destroy font (%s)", *Name);

	RpgPlatformMemory::MemFree(Unicode.PackedChars);
}


RpgPointInt RpgFont::CalculateTextDimension(const char* text, int length) const noexcept
{
	if (text == nullptr || length == 0)
	{
		return RpgPointInt();
	}

	const stbtt_packedchar* packedChars = static_cast<stbtt_packedchar*>(Unicode.PackedChars);

	RpgPointInt dim(0, Metric.Ascent - Metric.Descent);
	int x = dim.X;
	int y = dim.Y;

	for (int i = 0; i < length; ++i)
	{
		char c = text[i];
		RPG_PLATFORM_Assert(c);

		if (c == '\n')
		{
			y += Metric.LineSpace;
			dim.X = RpgMath::Max(dim.X, x);
			x = 0;

			continue;
		}

		if (c == '\t')
		{
			x += RPG_FONT_TAB_INDENT_PX;
			continue;
		}

		const int w = static_cast<int>(packedChars[c - Unicode.CodeStart].xadvance);
		RPG_PLATFORM_Assert(w > 0);
		x += w;
	}

	dim.X = RpgMath::Max(dim.X, x);
	dim.Y = RpgMath::Max(dim.Y, y);

	return dim;
}


int RpgFont::GenerateTextVertex(const char* text, int length, int x, int y, RpgColorRGBA color, RpgVertexMesh2DArray& out_Vertexes, RpgVertexIndexArray& out_Indexes, int* optOut_VertexCount, int* optOut_IndexCount) const noexcept
{
	if (optOut_VertexCount)
	{
		*optOut_VertexCount = 0;
	}

	if (optOut_IndexCount)
	{
		*optOut_IndexCount = 0;
	}

	if (text == nullptr || length == 0)
	{
		return 0;
	}

	const stbtt_packedchar* packedChars = static_cast<stbtt_packedchar*>(Unicode.PackedChars);
	const RpgPointInt texDim = Texture->GetDimension();

	float px = static_cast<float>(x);
	float py = static_cast<float>(y);

	uint32_t vertexCount = 0;
	int indexCount = 0;
	int charCount = 0;

	for (int i = 0; i < length; ++i)
	{
		char c = text[i];
		RPG_PLATFORM_Assert(c);

		if (c == '\n')
		{
			px = static_cast<float>(x);
			py += static_cast<float>(Metric.LineSpace);
			continue;
		}

		if (c == '\t')
		{
			px += static_cast<float>(RPG_FONT_TAB_INDENT_PX);
			continue;
		}

		stbtt_aligned_quad quad;
		stbtt_GetPackedQuad(packedChars, texDim.X, texDim.Y, c - Unicode.CodeStart, &px, &py, &quad, 1);
		RPG_PLATFORM_Assert(quad.x1 - quad.x0 >= 0.0f);
		RPG_PLATFORM_Assert(quad.y1 - quad.y0 >= 0.0f);

		const RpgVertex::FMesh2D vertexes[4] =
		{
			{ DirectX::XMFLOAT2(quad.x0, quad.y0), DirectX::XMFLOAT2(quad.s0, quad.t0), color },
			{ DirectX::XMFLOAT2(quad.x1, quad.y0), DirectX::XMFLOAT2(quad.s1, quad.t0), color },
			{ DirectX::XMFLOAT2(quad.x1, quad.y1), DirectX::XMFLOAT2(quad.s1, quad.t1), color },
			{ DirectX::XMFLOAT2(quad.x0, quad.y1), DirectX::XMFLOAT2(quad.s0, quad.t1), color },
		};

		out_Vertexes.InsertAtRange(vertexes, 4, RPG_INDEX_LAST);

		const RpgVertex::FIndex indexes[6] =
		{
			vertexCount,
			vertexCount + 1,
			vertexCount + 2,
			vertexCount + 2,
			vertexCount + 3,
			vertexCount
		};

		out_Indexes.InsertAtRange(indexes, 6, RPG_INDEX_LAST);

		vertexCount += 4;
		indexCount += 6;
		++charCount;
	}

	if (optOut_VertexCount)
	{
		*optOut_VertexCount = static_cast<int>(vertexCount);
	}

	if (optOut_IndexCount)
	{
		*optOut_IndexCount = indexCount;
	}

	return charCount;
}





static RpgArray<RpgSharedFont> DefaultFonts;


RpgSharedFont RpgFont::s_CreateShared(const RpgName& name, const RpgString& ttfFilePath, int sizePx, int unicodeStart, int unicodeCount) noexcept
{
	return RpgSharedFont(new RpgFont(name, ttfFilePath, sizePx, unicodeStart, unicodeCount));
}


void RpgFont::s_CreateDefaults() noexcept
{
	RPG_PLATFORM_LogDebug(RpgLogFont, "Create default fonts...");

	const RpgString fontAssetDirPath = RpgFileSystem::GetAssetRawDirPath() + "font/";

	DefaultFonts.AddValue(s_CreateShared("FNT_DEF_Roboto", fontAssetDirPath + "Roboto-Regular.ttf", 15, 32, 96));
	DefaultFonts.AddValue(s_CreateShared("FNT_DEF_ShareTechMono", fontAssetDirPath + "ShareTechMono-Regular.ttf", 15, 32, 96));
}


void RpgFont::s_DestroyDefaults() noexcept
{
	DefaultFonts.Clear(true);
}


const RpgSharedFont& RpgFont::s_GetDefault_Roboto() noexcept
{
	return DefaultFonts[0];
}


const RpgSharedFont& RpgFont::s_GetDefault_ShareTechMono() noexcept
{
	return DefaultFonts[1];
}
