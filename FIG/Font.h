#pragma once
#include "FreeType.h"
#include <map>
#include "EngineShared.h"

namespace FIG
{
    class FontRenderer;

    class ENGINE_SHARED Font
    {
    public:
        Font(const char * const filename, long faceIndex = 0);
        ~Font();

        FT_Face face;
        FontRenderer* CreateRenderer(float fontsize, FT_Int32 loadFlags = FT_LOAD_DEFAULT, int filterMode = 0);

    private:
        FreeType* ft;

        const char * const filename;
        FT_Int32 loadFlags;
        int filterMode;
    };
}