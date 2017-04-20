#pragma once

#include "EngineShared.h"
#include "FontRendererSettings.h"
#include "BoundingBox.h"

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
        void Draw(const char * const text, TArgs... args)
        {
            return Draw(defaultColorFg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&colorFg)[4], const char * const text, TArgs... args)
        {
            float colorBg[4]{ colorFg[0], colorFg[1], colorFg[2], 0.0 };
            return Draw(colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&colorFg)[4], const float(&colorBg)[4], const char * const text, TArgs... args)
        {
            float transform[16] = {
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0,
            };
            return Draw(transform, colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&transform)[16], const char * const text, TArgs... args)
        {
            return Draw(transform, defaultColorFg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&transform)[16], const float(&colorFg)[4], const char * const text, TArgs... args)
        {
            float colorBg[4]{ colorFg[0], colorFg[1], colorFg[2], 0.0 };
            return Draw(transform, colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&transform)[16], const float(&colorFg)[4], const float(&colorBg)[4], const char * const text, TArgs... args)
        {
            char buffer[1024];
            sprintf_s(buffer, text, args...);
            Draw(transform, colorFg, colorBg, buffer);
        }
        void Draw(const float(&transform)[16], const float(&colorFg)[4], const float(&colorBg)[4], const char * const text);


        template<typename... TArgs>
        void DrawDirect(int x, int y, const char * const text, TArgs... args)
        {
            DrawDirect(x, y, defaultColorFg, text, args...);
        }
        template<typename... TArgs>
        void DrawDirect(int x, int y, const float(&colorFg)[4], const char * const text, TArgs... args)
        {
            float colorBg[4]{ colorFg[0], colorFg[1], colorFg[2], 0.0 };
            DrawDirect(x, y, colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void DrawDirect(int x, int y, const float(&colorFg)[4], const float(&colorBg)[4], const char * const text, TArgs... args)
        {
            char buffer[1024];
            sprintf_s(buffer, text, args...);
            DrawDirect(x, y, colorFg, colorBg, buffer);
        }
        void DrawDirect(int x, int y, const float(&colorFg)[4], const float(&colorBg)[4], const char * const text);

        BoundingBox Bounds(const char * const text);
        BoundingBox RenderBounds(const char * const text);

        inline FontRendererImpl* Impl() { return impl; }

    private:
        FontRendererImpl* impl;

    };
}