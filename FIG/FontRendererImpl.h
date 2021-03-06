#pragma once

#include "Font.h"
#include "FontRendererSettings.h"
#include "BoundingBox.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#pragma warning (disable : 4003)

#define ReturnIfGLError(ret_val) if(CheckGLError())return ret_val

namespace FIG
{
    class ENGINE_SHARED FontRendererImpl
    {
    public:
        FontRendererImpl(Font* font, FontRendererSettings settings);
        ~FontRendererImpl();

        inline FontRendererSettings Settings()
        {
            return settings;
        }

        void Draw(FontDrawSettings drawSettings, const char * const text);

        BoundingBox Bounds(const char * const text);
        BoundingBox RenderBounds(const char * const text);

    private:
        void DrawInternal(FontDrawSettings drawSettings, const char * const text);
        void DrawDirect(FontDrawSettings drawSettngs, const char * const text);
        
        Font* font;

        FontRendererSettings settings;

        char * error = nullptr;
        static const size_t errorLength = 1024;
        template<typename... TArgs>
        void SetError(const char * const format, TArgs... args)
        {
            if (format == nullptr)
                error = nullptr;
            else
            {
                if (error != nullptr)
                    delete[] error;
                error = new char[errorLength];
                sprintf_s(error, errorLength, format, args...);
            }
        }

        bool CheckGLError();

        static const float defaultColorFg[4];
        static const float defaultColorBg[4];

        FT_UInt glyphIndeces[256];
        unsigned texture;
        FT_Glyph_Metrics metrics[256] = { { 0, 0, 0, 0, 0, 0, 0, 0 } };
        unsigned charWidth = 0;
        unsigned charHeight = 0;

        unsigned shader;

        bool CreateGlyphs();
        bool CreateGlyph(FT_ULong charCode, FT_UInt glyphIndex, unsigned char* bitmapData);
        
        static unsigned NextPowerOfTwo(unsigned n);
        void GetCharacterPositions(const char* const text, size_t textLength, float* points);

        unsigned rawPositionsVbo;
        unsigned charactersVbo;
        unsigned positionsVbo;

        bool CreateShader(const char * const vertexSource, const char * const fragmentSource);
        bool SetShaderData();
        void SetRenderData(const float* transform, const float* colorFg, const float* colorBg, const char * const text, unsigned length, float* positions);
    };
    
    extern const char * const shaderVertexSource;
    extern  const char * const shaderFragmentSource;
}