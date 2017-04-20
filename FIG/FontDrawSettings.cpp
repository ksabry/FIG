#include "stdafx.h"
#include "FontDrawSettings.h"

namespace FIG
{
    FontDrawSettings::~FontDrawSettings()
    {
    }

    FontDrawSettings FontDrawSettings::Copy()
    {
        return FontDrawSettings(
            COLOR_FG, colorFg,
            COLOR_BG, colorBg,
            TRANSFORM, transform,
            DIRECT, direct,
            DIRECT_X, directX,
            DIRECT_Y, directY
        );
    }
}
