#pragma once

#include "EngineShared.h"
#include "FontSettings.h"
#include "FontRenderer.h"
#include "FontRendererSettings.h"
#include "FontRendererDrawSettings.h"

namespace FIG
{
    class FontImpl;

    class ENGINE_SHARED Font
    {
    public:
        Font(const char * const filename, FontSettings settings = FontSettings());
        template<typename... TArgs>
        Font(const char * const filename, TArgs... args)
            : Font(FontRenderer(font, FontSettings(args...)))
        {
        }
        ~Font();

        FontSettings Settings();
        char * Error();

        FontRenderer* CreateRenderer(FontRendererSettings settings);

        template<typename... TArgs>
        FontRenderer* CreateRenderer(TArgs... args)
        {
            return CreateRenderer(FontRendererSettings(args...));
        }

        template<typename... TArgs>
        void Draw(FontRendererDrawSettings settings, const char * const format, TArgs... args)
        {
            char* buffer = new char[2048];
            sprintf_s(buffer, 2048, format, args...);
            Draw(settings, buffer);
            delete[] buffer;
        }
        void Draw(FontRendererDrawSettings settings, const char * const text);

        void Font::Draw(FontRendererSettings renderSettings, FontDrawSettings drawSettings, const char * const text);

        inline FontImpl* Impl() { return impl; }

    private:
        FontImpl* impl;
    };
}