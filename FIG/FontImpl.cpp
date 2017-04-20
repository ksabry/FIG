#include "stdafx.h"
#include "FontImpl.h"
#include <map>
#include "OpenGl.h"
#include "FontRenderer.h"
#include "FontSettings.h"

namespace FIG
{
    FontImpl::FontImpl(const char * const filename, long faceIndex, FontSettings settings)
        : filename(filename), faceIndex(faceIndex), settings(settings)
    {
        LoadFace();
    }
    FontImpl::~FontImpl()
    {
    }

    FontRenderer* FontImpl::CreateRenderer(Font* self, FontRendererSettings settings)
    {
        return new FontRenderer(self, settings);
    }

    FT_Library FontImpl::CreateLibrary()
    {
        FT_Library result;
        FT_Error error = FT_Init_FreeType(&result);
        if (error)
            return nullptr;
        return result;
    }

    void FontImpl::LoadFace()
    {
        FT_Error error = FT_New_Face(library, filename, faceIndex, &face);
        if (error)
            SetError("FT Error (%d) when loading face from font %s, faceIndex %d", error, filename, faceIndex);
    }

    const FT_Library FontImpl::library = FontImpl::CreateLibrary();
}