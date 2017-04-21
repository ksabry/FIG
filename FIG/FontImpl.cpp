#include "stdafx.h"
#include "FontImpl.h"
#include <map>
#include "OpenGl.h"
#include "FontRenderer.h"
#include "FontSettings.h"

namespace FIG
{
    FontImpl::FontImpl(const char * const filename, FontSettings settings)
        : filename(filename), settings(settings)
    {
        LoadFace();
    }
    FontImpl::~FontImpl()
    {
        for (auto pair : rendererCache)
            delete pair.second;
    }

    FontRenderer* FontImpl::CreateRenderer(Font* self, FontRendererSettings settings)
    {
        auto cached = rendererCache.find(settings);
        if (cached == rendererCache.end())
        {
            auto renderer = new FontRenderer(self, settings);
            rendererCache[settings] = renderer;
            return renderer;
        }
        else
        {
            return cached->second;
        }
    }

    FT_Library FontImpl::CreateLibrary()
    {
        FT_Library result;
        FT_Error error = FT_Init_FreeType(&result);
        if (error)
            return nullptr;
        return result;
    }

    void FontImpl::Draw(Font* self, FontRendererDrawSettings settings, const char * const text)
    {
        auto renderer = CreateRenderer(self, settings.RendererSettings());
        renderer->Draw(settings.DrawSettings(), text);
    }

    void FontImpl::Draw(Font* self, FontRendererSettings renderSettings, FontDrawSettings drawSettings, const char * const text)
    {
        auto renderer = CreateRenderer(self, renderSettings);
        renderer->Draw(drawSettings, text);
    }

    void FontImpl::LoadFace()
    {
        FT_Error error = FT_New_Face(library, filename, settings.faceIndex, &face);
        if (error)
            SetError("FT Error (%d) when loading face from font %s, faceIndex %d", error, filename, settings.faceIndex);
    }

    const FT_Library FontImpl::library = FontImpl::CreateLibrary();
}