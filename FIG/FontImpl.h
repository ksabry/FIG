#pragma once
#include <map>
#include "EngineShared.h"
#include "FontSettings.h"
#include "FontRendererSettings.h"
#include "FontRenderer.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace FIG
{    
    class FontImpl
    {
    public:
        FontImpl(const char * const filename, long faceIndex = 0, FontSettings settings = FontSettings());
        ~FontImpl();

        FT_Face face;

        inline FontSettings Settings()
        {
            return settings;
        }

        inline char * Error()
        {
            return error;
        }

        FontRenderer* CreateRenderer(Font* self, FontRendererSettings settings);
        static FT_Library CreateLibrary();

    private:
        FontSettings settings;
        
        static const size_t errorLength = 1024;
        char * error = nullptr;
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
        
        const char * const filename;
        long faceIndex;
        
        FT_Int32 loadFlags;
        int filterMode;

        static const FT_Library library;

        void LoadFace();
    };
}