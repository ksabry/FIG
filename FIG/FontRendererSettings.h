#pragma once

#include "EngineShared.h"
#include "TextAlignment.h"
#include <assert.h>

namespace FIG
{
    template<int FIELD>
    struct _InitField {};

#define DECLARE_FIELD(val, fieldName) constexpr _InitField<val> fieldName; int constexpr _##fieldName##_FIELD = val;
#define ADD_FIELD(fieldName,innerField,type) template<> void setField<_InitField<_##fieldName##_FIELD>, type>(type value) { innerField = value; }

    DECLARE_FIELD(1, FONT_SIZE);
    DECLARE_FIELD(2, FILTER_MODE);
    DECLARE_FIELD(3, GLYPH_LOAD_FLAGS);
    DECLARE_FIELD(4, GLYPH_RENDER_MODE);
    DECLARE_FIELD(5, USE_KERNING);
    DECLARE_FIELD(6, ALIGNMENT);
    DECLARE_FIELD(7, FULL_BACKGROUND);

    struct ENGINE_SHARED FontRendererSettings
    {
    public:
        float fontSize;
        int filterMode;
        int glyphLoadFlags;
        int glyphRenderMode;
        bool useKerning;
        TextAlignment alignment;
        bool fullBackground;

        template<typename... TArgs>
        FontRendererSettings(TArgs... args)
            : fontSize(0), filterMode(0), glyphLoadFlags(0), glyphRenderMode(0), useKerning(true), alignment(TextAlignment::Left), fullBackground(true)
        {
            init(args...);
            fillDefaults();
        }

        template<typename TField, typename TVal, typename... TArgs>
        void init(TField, TVal val, TArgs... args)
        {
            setField<TField>(val);
            init(args...);
        }
        template<typename TField, typename TVal>
        void init(TField, TVal val) 
        {
            setField<TField>(val);
        };

        template<typename TField, typename TVal>
        void setField(TVal val)
        {
            static_assert(false, "Unrecognized field in FontRendererSettings initialization");
        }

        template<> void setField<_InitField<_FONT_SIZE_FIELD>, float>(float value) { fontSize = value; }

        //ADD_FIELD(FONT_SIZE, fontSize, float)
        //ADD_FIELD(FILTER_MODE, filterMode, int)
        //ADD_FIELD(GLYPH_LOAD_FLAGS, glyphLoadFlags, int)
        //ADD_FIELD(GLYPH_RENDER_MODE, glyphRenderMode, int)
        //ADD_FIELD(USE_KERNING, useKerning, bool)
        //ADD_FIELD(ALIGNMENT, alignment, TextAlignment)
        //ADD_FIELD(FULL_BACKGROUND, fullBackground, bool)

    private:
        void fillDefaults();
    };

#undef FIELD_OF
#undef DECLARE_FIELD
#undef ADD_FIELD
}