#include "stdafx.h"
#include "Font.h"
#include "FontImpl.h"

namespace FIG
{
    Font::Font(const char * const filename, long faceIndex, FontSettings settings)
    {
        impl = new FontImpl(filename, faceIndex, settings);
    }

    Font::~Font()
    {
        delete impl;
    }

    FontSettings Font::Settings()
    {
        return impl->Settings();
    }

    char * Font::Error()
    {
        return impl->Error();
    }
    FontRenderer* Font::CreateRenderer(FontRendererSettings settings)
    {
        return impl->CreateRenderer(this, settings);
    }
}
