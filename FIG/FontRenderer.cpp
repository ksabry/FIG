#include "stdafx.h"
#include "FontRenderer.h"

#include "FontRendererImpl.h"

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

    void FontRenderer::Draw(const float(&transform)[16], const float(&colorFg)[4], const float(&colorBg)[4], const char * const text)
    {
        impl->Draw(transform, colorFg, colorBg, text);
    }

    void FontRenderer::DrawDirect(int x, int y, const float(&colorFg)[4], const float(&colorBg)[4], const char * const text)
    {
        impl->DrawDirect(x, y, colorFg, colorBg, text);
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
