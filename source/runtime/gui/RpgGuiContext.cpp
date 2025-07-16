#include "RpgGuiContext.h"


RPG_LOG_DEFINE_CATEGORY(RpgLogGUI, VERBOSITY_DEBUG)




RpgGuiContext::RpgGuiContext() noexcept
{
}


void RpgGuiContext::Begin(RpgRectInt rect) noexcept
{
	Rect = rect;

}


void RpgGuiContext::End(RpgRenderer2D& renderer) noexcept
{
}


void RpgGuiContext::AddRect(RpgPointInt dimension, RpgColorRGBA color, const RpgSharedTexture2D& texture, const RpgSharedMaterial& material) noexcept
{
	
}
