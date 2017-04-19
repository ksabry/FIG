#pragma once
#include <map>
#include "EngineShared.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "FontSettings.h"
#include "FontRendererSettings.h"

namespace FIG
{
    class FontRenderer;

    class ENGINE_SHARED Font
    {
    public:
        Font(const char * const filename, long faceIndex = 0, FontSettings settings = FontSettings());
        template<typename... TArgs>
        Font(const char * const filename, long faceIndex = 0, TArgs... args)
            : FontRenderer(font, FontSettings(args...))
        {
        }
        ~Font();

        FontSettings settings;

        char * error = nullptr;
        static const size_t errorLength = 1024;
        template<typename... TArgs>
        void SetError(const char * const format, TArgs... args)
        {
            if (format == nullptr)
                error = nullptr;
            else
            {
                error = new char[errorLength];
                sprintf_s(error, errorLength, format, args...);
            }
        }

        FT_Face face;
        FontRenderer* CreateRenderer(FontRendererSettings settings);
        template<typename... TArgs>
        FontRenderer* CreateRenderer(TArgs... args)
        {
            return CreateRenderer(FontRendererSettings(args...));
        }
        static FT_Library CreateLibrary();

    private:
        const char * const filename;
        long faceIndex;
        FT_Int32 loadFlags;
        int filterMode;

        static const FT_Library library;

        void LoadFace();
    };
}