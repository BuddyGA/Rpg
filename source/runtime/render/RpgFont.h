#pragma once

#include "core/RpgVertex.h"
#include "RpgTexture.h"


// Font size small
#define RPG_FONT_SIZE_SMALL		10

// Font size medium
#define RPG_FONT_SIZE_MEDIUM	15

// Font size large
#define RPG_FONT_SIZE_LARGE		20

// Font tab indent pixel size
#define RPG_FONT_TAB_INDENT_PX	16



struct RpgFontUnicodeRange
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

public:
	RpgFont(const RpgName& in_Name, const RpgString& ttfFilePath, int in_SizePx, int in_UnicodeStart, int in_UnicodeCount) noexcept;
	~RpgFont() noexcept;

	RpgPointInt CalculateTextDimension(const char* text, int length) const noexcept;
	int GenerateTextVertex(const char* text, int length, int x, int y, RpgColorRGBA color, RpgVertexMesh2DArray& out_Vertexes, RpgVertexIndexArray& out_Indexes, int* optOut_VertexCount = nullptr, int* optOut_IndexCount = nullptr) const noexcept;


	inline const RpgFontUnicodeRange& GetUnicodeRange() const noexcept
	{
		return UnicodeRange;
	}

	inline RpgFontMetric GetMetric() const noexcept
	{
		return Metric;
	}

	inline const RpgSharedTexture2D& GetTexture() const noexcept
	{
		return Texture;
	}


private:
	RpgName Name;
	RpgFontUnicodeRange UnicodeRange;
	RpgFontMetric Metric;
	RpgSharedTexture2D Texture;


public:
	[[nodiscard]] static RpgSharedFont s_CreateShared(const RpgName& name, const RpgString& ttfFilePath, int sizePx, int unicodeStart, int unicodeCount) noexcept;

	static void s_CreateDefaults() noexcept;
	static void s_DestroyDefaults() noexcept;

	[[nodiscard]] static const RpgSharedFont& s_GetDefault_Roboto() noexcept;
	[[nodiscard]] static const RpgSharedFont& s_GetDefault_ShareTechMono() noexcept;

};
