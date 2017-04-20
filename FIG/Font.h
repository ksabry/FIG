#pragma once

#include "EngineShared.h"
#include "FontSettings.h"
#include "FontRenderer.h"
#include "FontRendererSettings.h"

namespace FIG
{
    class FontImpl;

    class ENGINE_SHARED Font
    {
    public:
        Font(const char * const filename, long faceIndex = 0, FontSettings settings = FontSettings());
        template<typename... TArgs>
        Font(const char * const filename, long faceIndex = 0, TArgs... args)
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

        inline FontImpl* Impl() { return impl; }

    private:
        FontImpl* impl;
    };
}