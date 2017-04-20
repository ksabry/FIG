#pragma once

#include "Font.h"
#include "FontRendererSettings.h"
#include "BoundingBox.h"

#pragma warning (disable : 4003)

#define ReturnIfGLError(ret_val) if(CheckGLError())return ret_val

namespace FIG
{
    class ENGINE_SHARED FontRenderer
    {
    public:
        FontRenderer(Font* font, FontRendererSettings settings);
        template<typename... TArgs>
        FontRenderer(Font* font, TArgs... args)
            : FontRenderer(font, FontRendererSettings(args...))
        {
        }
        ~FontRenderer();

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
                error = new char[errorLength];
                sprintf_s(error, errorLength, format, args...);
            }
        }

        bool CheckGLError();

        static const float defaultColorFg[4];
        static const float defaultColorBg[4];

        template<typename... TArgs>
        void DrawDirect(int x, int y, const char * const text, TArgs... args)
        {
            DrawDirect(x, y, defaultColorFg, text, args...);
        }
        template<typename... TArgs>
        void DrawDirect(int x, int y, const float(&colorFg)[4], const char * const text, TArgs... args)
        {
            float colorBg[4]{ colorFg[0], colorFg[1], colorFg[2], 0.0 };
            DrawDirect(x, y, colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void DrawDirect(int x, int y, const float(&colorFg)[4], const float(&colorBg)[4], const char * const text, TArgs... args)
        {
            char buffer[1024];
            sprintf_s(buffer, text, args...);
            DrawDirect(x, y, colorFg, colorBg, buffer);
        }
        void DrawDirect(int x, int y, const float(&colorFg)[4], const float(&colorBg)[4], const char * const text);

        template<typename... TArgs>
        void Draw(const char * const text, TArgs... args)
        {
            return Draw(defaultColorFg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&colorFg)[4], const char * const text, TArgs... args)
        {
            float colorBg[4]{ colorFg[0], colorFg[1], colorFg[2], 0.0 };
            return Draw(colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&colorFg)[4], const float(&colorBg)[4], const char * const text, TArgs... args)
        {
            float transform[16] = {
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0,
            };
            return Draw(transform, colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&transform)[16], const char * const text, TArgs... args)
        {
            return Draw(transform, defaultColorFg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&transform)[16], const float(&colorFg)[4], const char * const text, TArgs... args)
        {
            float colorBg[4]{ colorFg[0], colorFg[1], colorFg[2], 0.0 };
            return Draw(transform, colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void Draw(const float(&transform)[16], const float(&colorFg)[4], const float(&colorBg)[4], const char * const text, TArgs... args)
        {
            char buffer[1024];
            sprintf_s(buffer, text, args...);
            Draw(transform, colorFg, colorBg, buffer);
        }
        void Draw(const float(&transform)[16], const float(&colorFg)[4], const float(&colorBg)[4], const char * const text);

        BoundingBox Bounds(const char * const text);

        BoundingBox RenderBounds(const char * const text);

    private:
        Font* font;

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
        void SetRenderData(const float(&transform)[16], const float(&colorFg)[4], const float(&colorBg)[4], const char * const text, unsigned length, float* positions);
    };
    
    extern const char * const shaderVertexSource;
    extern  const char * const shaderFragmentSource;
}