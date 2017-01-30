#include "stdafx.h"
#include "FontRendererSettings.h"
#include "OpenGl.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace FIG
{
    FontRendererSettings::FontRendererSettings(float fontSize, int filterMode, int glyphLoadFlags)
        : fontSize(fontSize), filterMode(filterMode ? filterMode : GL_LINEAR), glyphLoadFlags(glyphLoadFlags ? glyphLoadFlags : FT_LOAD_DEFAULT)
    {
    }
}