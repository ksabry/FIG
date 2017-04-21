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
            : fontSize(0.f), filterMode(0), glyphLoadFlags(0), glyphRenderMode(0), useKerning(true), alignment(TextAlignment::Left), fullBackground(true)
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

        static friend bool operator== (const FontRendererSettings& left, const FontRendererSettings& right)
        {
            return
                left.fontSize == right.fontSize &&
                left.filterMode == right.filterMode &&
                left.glyphLoadFlags == right.glyphLoadFlags &&
                left.glyphRenderMode == right.glyphRenderMode &&
                left.useKerning == right.useKerning &&
                left.alignment == right.alignment &&
                left.fullBackground == right.fullBackground;
        }

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

namespace std
{
    template<>
    struct hash<FIG::FontRendererSettings>
    {
        size_t operator() (FIG::FontRendererSettings const& settings) const
        {
            size_t result =
                (*reinterpret_cast<const size_t*>(&settings.useKerning))            ^
                (*reinterpret_cast<const size_t*>(&settings.fullBackground)  << 1)  ^
                (*reinterpret_cast<const size_t*>(&settings.alignment)       << 2)  ^
                (*reinterpret_cast<const size_t*>(&settings.filterMode)      << 6)  ^
                (*reinterpret_cast<const size_t*>(&settings.glyphLoadFlags)  << 10) ^
                (*reinterpret_cast<const size_t*>(&settings.glyphRenderMode) << 14) ^
                (*reinterpret_cast<const size_t*>(&settings.fontSize)        << 18) ;
            
            return result;
        }
    };
}
