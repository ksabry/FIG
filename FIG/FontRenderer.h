#pragma once
#include "Font.h"

namespace FIG
{
    class ENGINE_SHARED FontRenderer
    {
    public:
        FontRenderer(Font* font, float fontsize, FT_Int32 loadFlags = FT_LOAD_DEFAULT, int filterMode = 0);
        ~FontRenderer();

        char * error = nullptr;
        static const size_t errorLength = 4096;
        template<typename... TArgs>
        void PutError(const char * const format, TArgs... args)
        {
            if (format == nullptr)
                error = nullptr;
            else
            {
                error = new char[errorLength];
                sprintf_s(error, errorLength, format, args...);
            }
        }

        template<typename... TArgs>
        void Draw(const char * const text, TArgs... args)
        {
            float colorFg[4] = { 1.0, 1.0, 1.0, 1.0 };
            return Draw(colorFg, text, args...);
        }
        template<typename... TArgs>
        void Draw(float(&colorFg)[4], const char * const text, TArgs... args)
        {
            float colorBg[4] = { 0.0, 0.0, 0.0, 0.0 };
            return Draw(colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void Draw(float(&colorFg)[4], float(&colorBg)[4], const char * const text, TArgs... args)
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
        void Draw(float(&transform)[16], const char * const text, TArgs... args)
        {
            float colorFg[4] = { 1.0, 1.0, 1.0, 1.0 };
            return Draw(transform, colorFg, text, args...);
        }
        template<typename... TArgs>
        void Draw(float(&transform)[16], float(&colorFg)[4], const char * const text, TArgs... args)
        {
            float colorBg[4] = {0.0, 0.0, 0.0, 0.0};
            return Draw(transform, colorFg, colorBg, text, args...);
        }
        template<typename... TArgs>
        void Draw(float(&transform)[16], float(&colorFg)[4], float(&colorBg)[4], const char * const text, TArgs... args)
        {
            char buffer[1024];
            sprintf_s(buffer, text, args...);
            Draw(transform, colorFg, colorBg, buffer);
        }
        void Draw(float(&transform)[16], float(&colorFg)[4], float(&colorBg)[4], const char * const text);

    private:
        Font* font;
        unsigned fontsize;
        FT_Int32 loadFlags;
        int filterMode;

        FT_UInt glyphIndeces[256];
        unsigned texture;
        FT_Glyph_Metrics metrics[256] = { { 0, 0, 0, 0, 0, 0, 0, 0 } };
        unsigned charWidth = 0;
        unsigned charHeight = 0;

        unsigned shader;

        bool CreateGlyphs();
        bool CreateGlyph(FT_ULong charCode, FT_UInt glyphIndex, unsigned char* bitmapData);
        
        static unsigned nextPowerOfTwo(unsigned n);
        void GetCharacterPositions(const char* const text, size_t textLength, float* points);

        bool CreateShader(const char * const vertexSource, const char * const fragmentSource);
        void SetUniforms(float(&transform)[16], float(&colorFg)[4], float(&colorBg)[4], const char * const text, unsigned length, float* positions);
    };
    
    extern const char * const shaderVertexSource;
    extern  const char * const shaderFragmentSource;
}