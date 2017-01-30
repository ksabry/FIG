#pragma once
#include "ft2build.h"
#include FT_FREETYPE_H
#include "EngineShared.h"

namespace FIG
{
    struct ENGINE_SHARED FontSettings
    {
        FT_Int32 freetypeLoadFlags;

        FontSettings(FT_Int32 freetypeLoadFlags = FT_LOAD_DEFAULT);
    };
}