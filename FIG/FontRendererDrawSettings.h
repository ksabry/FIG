#pragma once

#include "EngineShared.h"
#include "FieldInitializable.h"

#include "FontRendererSettings.h"
#include "FontDrawSettings.h"

namespace FIG
{
    class ENGINE_SHARED FontRendererDrawSettings : public FieldInitializable<FontRendererDrawSettings>
    {
    public:
        template<typename... TArgs>
        FontRendererDrawSettings(TArgs... args)
        {
            FontRendererSettings renderSettings = FontRendererSettings();
            FontDrawSettings drawSettings = FontDrawSettings();

            fontSize = renderSettings.fontSize;
            filterMode = renderSettings.filterMode;
            glyphLoadFlags = renderSettings.glyphLoadFlags;
            glyphRenderMode = renderSettings.glyphRenderMode;
            useKerning = renderSettings.useKerning;
            alignment = renderSettings.alignment;
            fullBackground = renderSettings.fullBackground;

            colorBg = drawSettings.colorBg;
            colorFg = drawSettings.colorFg;
            transform = drawSettings.transform;
            direct = drawSettings.direct;
            directX = drawSettings.directX;
            directY = drawSettings.directY;

            FieldInitializable::Init(args...);
        }
        ~FontRendererDrawSettings();

        float fontSize;
        int filterMode;
        int glyphLoadFlags;
        int glyphRenderMode;
        bool useKerning;
        TextAlignment alignment;
        bool fullBackground;

        const float* colorBg;
        const float* colorFg;
        const float* transform;
        bool direct;
        int directX;
        int directY;

        FontRendererSettings RendererSettings();
        FontDrawSettings DrawSettings();
    };

    ADD_EXISTING_FIELD(float, FONT_SIZE, FontRendererDrawSettings, fontSize);
    ADD_EXISTING_FIELD(int, FILTER_MODE, FontRendererDrawSettings, filterMode);
    ADD_EXISTING_FIELD(int, GLYPH_LOAD_FLAGS, FontRendererDrawSettings, glyphLoadFlags);
    ADD_EXISTING_FIELD(int, GLYPH_RENDER_MODE, FontRendererDrawSettings, glyphRenderMode);
    ADD_EXISTING_FIELD(bool, USE_KERNING, FontRendererDrawSettings, useKerning);
    ADD_EXISTING_FIELD(TextAlignment, ALIGNMENT, FontRendererDrawSettings, alignment);
    ADD_EXISTING_FIELD(bool, FULL_BACKGROUND, FontRendererDrawSettings, fullBackground);

    ADD_EXISTING_FIELD(const float*, COLOR_BG, FontRendererDrawSettings, colorBg);
    ADD_EXISTING_FIELD(const float*, COLOR_FG, FontRendererDrawSettings, colorFg);
    ADD_EXISTING_FIELD(const float*, TRANSFORM, FontRendererDrawSettings, transform);
    ADD_EXISTING_FIELD(bool, DIRECT, FontRendererDrawSettings, direct);
    ADD_EXISTING_FIELD(int, DIRECT_X, FontRendererDrawSettings, directX);
    ADD_EXISTING_FIELD(int, DIRECT_Y, FontRendererDrawSettings, directY);
}