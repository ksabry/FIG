#include "stdafx.h"
#include "FontRendererSettings.h"
#include "OpenGl.h"

namespace FIG
{
    FontRendererSettings::~FontRendererSettings()
    {
    }
    
    void FontRendererSettings::SetDefaults()
    {
        if (filterMode == 0)
            filterMode = GL_LINEAR;
    }
}
