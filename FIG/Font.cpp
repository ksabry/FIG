#include "stdafx.h"
#include "Font.h"
#include <map>
#include "OpenGl.h"
#include "FontRenderer.h"
#include "FontSettings.h"

namespace FIG
{
    Font::Font(const char * const filename, long faceIndex, FontSettings settings)
        : filename(filename), faceIndex(faceIndex), settings(settings)
    {
        LoadFace();
    }
    Font::~Font()
    {
    }

    FontRenderer* Font::CreateRenderer(FontRendererSettings settings)
    {
        return new FontRenderer(this, settings);
    }

    FT_Library Font::CreateLibrary()
    {
        FT_Library result;
        FT_Error error = FT_Init_FreeType(&result);
        if (error)
            return nullptr;
        return result;
    }

    void Font::LoadFace()
    {
        FT_Error error = FT_New_Face(library, filename, faceIndex, &face);
        if (error)
            SetError("FT Error (%d) when loading face from font %s, faceIndex %d", error, filename, faceIndex);
    }

    const FT_Library Font::library = Font::CreateLibrary();
}