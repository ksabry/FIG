#include "stdafx.h"
#include "FontRenderer.h"

#include "FontRendererImpl.h"
#include "FontDrawSettings.h"

namespace FIG
{
    FontRenderer::FontRenderer(Font* font, FontRendererSettings settings)
    {
        impl = new FontRendererImpl(font, settings);
    }

    FontRenderer::~FontRenderer()
    {
        delete impl;
    }

    FontRendererSettings FontRenderer::Settings()
    {
        return impl->Settings();
    }

    void FontRenderer::Draw(FontDrawSettings drawSettings, const char * const text)
    {
        impl->Draw(drawSettings, text);
    }

    BoundingBox FontRenderer::Bounds(const char * const text)
    {
        return impl->Bounds(text);
    }

    BoundingBox FontRenderer::RenderBounds(const char * const text)
    {
        return impl->RenderBounds(text);
    }
}
