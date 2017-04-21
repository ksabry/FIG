#include "stdafx.h"
#include "Font.h"
#include "FontImpl.h"

namespace FIG
{
    Font::Font(const char * const filename, FontSettings settings)
    {
        impl = new FontImpl(filename, settings);
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

    void Font::Draw(FontRendererDrawSettings settings, const char * const text)
    {
        return impl->Draw(this, settings, text);
    }

    void Font::Draw(FontRendererSettings renderSettings, FontDrawSettings drawSettings, const char * const text)
    {
        return impl->Draw(this, renderSettings, drawSettings, text);
    }
}
