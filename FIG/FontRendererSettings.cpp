#include "stdafx.h"
#include "FontRendererSettings.h"
#include "OpenGl.h"

namespace FIG
{
    void FontRendererSettings::fillDefaults()
    {
        if (filterMode == 0)
            filterMode = GL_LINEAR;
    }
}
