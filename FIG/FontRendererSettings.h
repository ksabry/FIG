#pragma once

#include "EngineShared.h"

namespace FIG
{
    struct ENGINE_SHARED FontRendererSettings
    {
        float fontSize;
        int filterMode;
        int glyphLoadFlags;

        FontRendererSettings(float fontSize, int filterMode = 0, int glyphLoadFlags = 0);
    };
}