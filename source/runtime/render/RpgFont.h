#pragma once

#include "core/RpgVertex.h"
#include "RpgTexture2D.h"



struct RpgFontUnicode
{
	int CodeStart{ 0 };
	int CodeCount{ 0 };
	void* PackedChars{ nullptr };
};


struct RpgFontMetric
{
	int SizePx{ 0 };
	int Ascent{ 0 };
	int Descent{ 0 };
	int LineSpace{ 0 };
};



typedef RpgSharedPtr<class RpgFont> RpgSharedFont;

class RpgFont
{
	RPG_NOCOPY(RpgFont)

private:
	RpgName Name;
	RpgFontUnicode Unicode;
	RpgFontMetric Metric;
	RpgSharedTexture2D Texture;


private:
	RpgFont(const RpgName& in_Name, const RpgString& ttfFilePath, int in_SizePx, int in_UnicodeStart, int in_UnicodeCount) noexcept;

public:
	~RpgFont() noexcept;

	[[nodiscard]] RpgPointInt CalculateTextDimension(const char* text, int length) const noexcept;
	int GenerateTextVertex(const char* text, int length, int x, int y, RpgColorRGBA color, RpgVertexMesh2DArray& out_Vertexes, RpgVertexIndexArray& out_Indexes, int* optOut_VertexCount = nullptr, int* optOut_IndexCount = nullptr) const noexcept;


	[[nodiscard]] inline const RpgFontUnicode& GetUnicode() const noexcept
	{
		return Unicode;
	}

	[[nodiscard]] inline RpgFontMetric GetMetric() const noexcept
	{
		return Metric;
	}

	[[nodiscard]] inline const RpgSharedTexture2D& GetTexture() const noexcept
	{
		return Texture;
	}


public:
	[[nodiscard]] static RpgSharedFont s_CreateShared(const RpgName& name, const RpgString& ttfFilePath, int sizePx, int unicodeStart, int unicodeCount) noexcept;

	static void s_CreateDefaults() noexcept;
	static void s_DestroyDefaults() noexcept;

	[[nodiscard]] static const RpgSharedFont& s_GetDefault_Roboto() noexcept;
	[[nodiscard]] static const RpgSharedFont& s_GetDefault_ShareTechMono() noexcept;

};
