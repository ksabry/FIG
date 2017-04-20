#pragma once

#include "FieldInitializable.h"
#include "EngineShared.h"
#include "TextAlignment.h"
#include <assert.h>

namespace FIG
{
    struct ENGINE_SHARED FontRendererSettings : public FieldInitializable<FontRendererSettings>
    {
    public:
        template<typename... TArgs>
        FontRendererSettings(TArgs... args)
            : fontSize(0.f), filterMode(0), glyphLoadFlags(0), glyphRenderMode(0), useKerning(false), alignment(TextAlignment::Left), fullBackground(false)
        { 
            FieldInitializable::Init(args...);
            SetDefaults();
        }
        ~FontRendererSettings();


        float fontSize;
        int filterMode;
        int glyphLoadFlags;
        int glyphRenderMode;
        bool useKerning;
        TextAlignment alignment;
        bool fullBackground;

    private:
        void SetDefaults();
    };
    
    ADD_FIELD(float, FONT_SIZE, FontRendererSettings, fontSize);
    ADD_FIELD(int, FILTER_MODE, FontRendererSettings, filterMode);
    ADD_FIELD(int, GLYPH_LOAD_FLAGS, FontRendererSettings, glyphLoadFlags);
    ADD_FIELD(int, GLYPH_RENDER_MODE, FontRendererSettings, glyphRenderMode);
    ADD_FIELD(bool, USE_KERNING, FontRendererSettings, useKerning);
    ADD_FIELD(TextAlignment, ALIGNMENT, FontRendererSettings, alignment);
    ADD_FIELD(bool, FULL_BACKGROUND, FontRendererSettings, fullBackground);
}