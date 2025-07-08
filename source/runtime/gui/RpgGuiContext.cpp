#include "RpgGuiContext.h"
#include "thirdparty/clay/clay.h"


RPG_LOG_DEFINE_CATEGORY(RpgLogGUI, VERBOSITY_DEBUG)

#define RPG_GUI_FromClayColor(c)		RpgColorRGBA(static_cast<uint8_t>(c.r * 255), static_cast<uint8_t>(c.g * 255), static_cast<uint8_t>(c.b * 255), static_cast<uint8_t>(c.a * 255))
#define RPG_GUI_ToClayColor(c)			{ static_cast<float>(c.R) / 255.0f, static_cast<float>(c.G) / 255.0f, static_cast<float>(c.B) / 255.0f, static_cast<float>(c.A) / 255.0f }
#define RPG_GUI_FromClayBoundingBox(b)	RpgRectInt(static_cast<int>(b.x), static_cast<int>(b.y), static_cast<int>(b.x + b.width), static_cast<int>(b.y + b.height))



static void Rpg_ClayErrorHandler(Clay_ErrorData error) noexcept
{
	RPG_LogError(RpgLogGUI, "ClayError: %s", error.errorText.chars);
}




RpgGuiContext::RpgGuiContext() noexcept
{
	bClayInitialized = false;
}


void RpgGuiContext::Begin(RpgRectInt rect) noexcept
{
	Rect = rect;

	Clay_Dimensions dimensions{};
	dimensions.width = static_cast<float>(rect.GetWidth());
	dimensions.height = static_cast<float>(rect.GetHeight());

	if (!bClayInitialized)
	{
		const uint32_t minMemorySizeBytes = Clay_MinMemorySize();
		Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(minMemorySizeBytes, RpgPlatformMemory::MemMalloc(minMemorySizeBytes));
		Clay_Initialize(arena, dimensions, (Clay_ErrorHandler)Rpg_ClayErrorHandler);

		bClayInitialized = true;
	}

	Clay_SetLayoutDimensions(dimensions);
	Clay_BeginLayout();
}


void RpgGuiContext::End(RpgRenderer2D& renderer) noexcept
{
	Clay_RenderCommandArray renderCommands = Clay_EndLayout();

	for (int i = 0; i < renderCommands.length; ++i)
	{
		Clay_RenderCommand* cmd = &renderCommands.internalArray[i];
		const Clay_RenderCommandType type = cmd->commandType;

		switch (type)
		{
			// The renderer should draw a solid color rectangle.
			case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
			{
				Clay_RectangleRenderData* data = &cmd->renderData.rectangle;
				renderer.AddRect(RPG_GUI_FromClayBoundingBox(cmd->boundingBox), RPG_GUI_FromClayColor(data->backgroundColor));

				break;
			}

			// The renderer should draw a colored border inset into the bounding box.
			case CLAY_RENDER_COMMAND_TYPE_BORDER:
			{
				Clay_BorderRenderData* data = &cmd->renderData.border;

				break;
			}

			// The renderer should draw text.
			case CLAY_RENDER_COMMAND_TYPE_TEXT:
			{
				Clay_TextRenderData* data = &cmd->renderData.text;
				
				break;
			}

			// The renderer should draw an image.
			case CLAY_RENDER_COMMAND_TYPE_IMAGE:
			{
				Clay_ImageRenderData* data = &cmd->renderData.image;
				
				break;
			}

			// The renderer should begin clipping all future draw commands, only rendering content that falls within the provided boundingBox.
			case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
			{
				renderer.PushClipRect(RPG_GUI_FromClayBoundingBox(cmd->boundingBox));

				break;
			}

			// The renderer should finish any previously active clipping, and begin rendering elements in full again.
			case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
			{
				renderer.PopClipRect();

				break;
			}

			// The renderer should provide a custom implementation for handling this render command based on its .customData
			case CLAY_RENDER_COMMAND_TYPE_CUSTOM:
			{
				break;
			}

			default:
				break;
		}
	}
}


void RpgGuiContext::AddRect(RpgPointInt dimension, RpgColorRGBA color, const RpgSharedTexture2D& texture, const RpgSharedMaterial& material) noexcept
{
	CLAY({
		.layout = 
		{
			.sizing = 
			{
				.width = static_cast<float>(dimension.X),
				.height = static_cast<float>(dimension.Y)
			}
		},
		.backgroundColor = RPG_GUI_ToClayColor(color)
	});
}
