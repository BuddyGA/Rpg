#pragma once

#include "core/RpgVertex.h"
#include "render/RpgRenderer2D.h"
#include "RpgGuiTypes.h"



class RpgGuiContext
{
	RPG_NOCOPY(RpgGuiContext)

private:
	RpgRectInt Rect;
	RpgArray<RpgSharedFont, 4> Fonts;
	RpgArray<RpgSharedTexture2D, 4> Textures;
	bool bClayInitialized;


public:
	RpgGuiContext() noexcept;

	void Begin(RpgRectInt rect) noexcept;
	void End(RpgRenderer2D& renderer) noexcept;

	void AddRect(RpgPointInt dimension, RpgColorRGBA color, const RpgSharedTexture2D& texture = RpgSharedTexture2D(), const RpgSharedMaterial& material = RpgSharedMaterial()) noexcept;

};
