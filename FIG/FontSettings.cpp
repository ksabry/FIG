#include "stdafx.h"
#include "FontSettings.h"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace FIG
{
    FontSettings::~FontSettings()
    {
    }

    void FontSettings::SetDefaults()
    {
        if (freetypeLoadFlags == 0)
            freetypeLoadFlags = FT_LOAD_DEFAULT;
    }
}