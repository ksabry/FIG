#include "stdafx.h"
#include "FontRendererDrawSettings.h"

namespace FIG
{
    FontRendererDrawSettings::~FontRendererDrawSettings()
    {
    }
    
    FontRendererSettings FontRendererDrawSettings::RendererSettings()
    {
        auto result = FontRendererSettings(
            FONT_SIZE, fontSize,
            FILTER_MODE, filterMode,
            GLYPH_LOAD_FLAGS, glyphLoadFlags,
            GLYPH_RENDER_MODE, glyphRenderMode,
            USE_KERNING, useKerning,
            ALIGNMENT, alignment,
            FULL_BACKGROUND, fullBackground
        );
        result.SetDefaults();
        return result;
    }
    
    FontDrawSettings FontRendererDrawSettings::DrawSettings()
    {
        return FontDrawSettings(
            COLOR_BG, colorBg,
            COLOR_FG, colorFg,
            TRANSFORM, transform,
            DIRECT, direct,
            DIRECT_X, directX,
            DIRECT_Y, directY
        );
    }
}
