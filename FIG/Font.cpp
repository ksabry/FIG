#include "stdafx.h"
#include "Font.h"
#include <map>
#include "OpenGl.h"
#include "FontRenderer.h"

namespace FIG
{
    Font::Font(const char * const filename, long faceIndex)
        : ft(FreeType::GetInstance()), filename(filename), loadFlags(loadFlags), filterMode(filterMode == 0 ? GL_LINEAR : filterMode)
    {
        face = ft->LoadFace(filename, faceIndex);
    }
    Font::~Font()
    {
    }

    FontRenderer* Font::CreateRenderer(float fontsize, FT_Int32 loadFlags, int filterMode)
    {
        return new FontRenderer(this, fontsize, loadFlags, filterMode);
    }
}