#pragma once

#include "EngineShared.h"
#include "FontRendererSettings.h"
#include "BoundingBox.h"
#include "FontDrawSettings.h"

namespace FIG
{
    class Font;
    class FontRendererImpl;

    class ENGINE_SHARED FontRenderer
    {
    public:
        FontRenderer(Font* font, FontRendererSettings settings);
        template<typename... TArgs>
        FontRenderer(Font* font, TArgs... args)
            : FontRenderer(font, FontRendererSettings(args...))
        {
        }
        ~FontRenderer();

        FontRendererSettings Settings();

        template<typename... TArgs>
        void Draw(FontDrawSettings drawSettings, const char * const format, TArgs... args)
        {
            char* buffer = new char[2048];
            sprintf_s(buffer, 2048, format, args...);
            Draw(drawSettings, buffer);
            delete[] buffer;
        }
        void Draw(FontDrawSettings, const char * const text);

        BoundingBox Bounds(const char * const text);
        BoundingBox RenderBounds(const char * const text);

        inline FontRendererImpl* Impl() { return impl; }

    private:
        FontRendererImpl* impl;
    };
}