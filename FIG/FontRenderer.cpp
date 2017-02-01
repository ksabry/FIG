#include "stdafx.h"
#include "FontRenderer.h"

#include "OpenGl.h"
#include <assert.h>
#include <iostream>
#include FT_GLYPH_H

namespace FIG
{
    FontRenderer::FontRenderer(Font * font, FontRendererSettings settings)
        : font(font), settings(settings)
    {
        if (!CreateGlyphs())
            std::cout << error;
        if (!CreateShader(shaderVertexSource, shaderFragmentSource))
            std::cout << error;
        if (!SetShaderData())
            std::cout << error;
    }
    FontRenderer::~FontRenderer()
    {
    }

    void FontRenderer::Draw(float(&transform)[16], float(&colorFg)[4], float(&colorBg)[4], const char * const text)
    {
        size_t length = strlen(text);
        if (length == 0)
            return;

        float* points = new float[2 * length];
        GetCharacterPositions(text, length, points);

        glUseProgram(shader);
        glBindBuffer(GL_ARRAY_BUFFER, rawPositionsVbo);

        unsigned chunks = (length - 1) / 256 + 1;
        for (unsigned chunk = 0; chunk < chunks; chunk++)
        {
            SetUniforms(transform, colorFg, colorBg, text + chunk * 256, chunk == chunks - 1 ? (length - 1) % 256 + 1 : 256, points + chunk * 512);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, length);
        }
    }

    BoundingBox FontRenderer::Bounds(const char * const text)
    {
        size_t length = strlen(text);
        if (length == 0)
            return BoundingBox{0.f, 0.f, 0.f, 0.f};

        float* points = new float[2 * length];
        GetCharacterPositions(text, length, points);
        
        float minX = points[0], maxX = points[0], minY = points[1], maxY = points[1];
        for (size_t i = 1; i < length; i++)
        {
            float lowX = points[2 * i], lowY = points[2 * i + 1];
            float highX = points[2 * i] + metrics[text[i]].width / 64, highY = points[2 * i + 1] + metrics[text[i]].height / 64;

            minX = lowX < minX ? lowX : minX;
            minX = highX > maxX ? highX : maxX;
            minX = lowY < minY ? lowY : minY;
            minX = highY > maxY ? highY : maxY;
        }
        return BoundingBox{minX, maxX, minY, maxY};
    }

    bool FontRenderer::CreateGlyphs()
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_3D, texture);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, settings.filterMode);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, settings.filterMode);

        FT_F26Dot6 f266_fontsize = (long)(settings.fontSize * 64.f + 0.5f);
        FT_Set_Char_Size(font->face, f266_fontsize, f266_fontsize, 96, 96);

        FT_Pos bboxWidth = font->face->bbox.xMax - font->face->bbox.xMin;
        FT_Pos bboxHeight = font->face->bbox.yMax - font->face->bbox.yMin;
        charWidth = nextPowerOfTwo((unsigned)ceil(font->face->size->metrics.x_scale / 65536.0 * bboxWidth / 64.0));
        charHeight = nextPowerOfTwo((unsigned)ceil(font->face->size->metrics.y_scale / 65536.0 * bboxHeight / 64.0));

        GLubyte* bitmapData = new GLubyte[256 * charWidth * charHeight]{ '\0' };

        FT_UInt glyphIndex;
        FT_ULong charCode = FT_Get_First_Char(font->face, &glyphIndex);
        while (glyphIndex)
        {
            if (charCode < 256 && !CreateGlyph(charCode, glyphIndex, bitmapData))
                return false;
            charCode = FT_Get_Next_Char(font->face, charCode, &glyphIndex);
        }

        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, charWidth, charHeight, 256, 0, GL_RED, GL_UNSIGNED_BYTE, bitmapData);

        return true;
    }

    bool FontRenderer::CreateGlyph(FT_ULong charCode, FT_UInt glyphIndex, unsigned char* bitmapData)
    {
        FT_Error ftError = FT_Load_Glyph(font->face, glyphIndex, settings.glyphLoadFlags);
        if (ftError)
        {
            SetError("FT Error (%d) when loading glyph at index %d", ftError, glyphIndex);
            return false;
        }

        FT_Glyph glyph;
        ftError = FT_Get_Glyph(font->face->glyph, &glyph);
        if (ftError)
        {
            SetError("FT Error (%d) when getting glyph at index %d", ftError, glyphIndex);
            return false;
        }

        FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
        FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
        FT_Bitmap& bitmap = bitmapGlyph->bitmap;

        for (unsigned y = 0; y < bitmap.rows; y++)
            for (unsigned x = 0; x < bitmap.width; x++)
                bitmapData[x + (y + charCode * charHeight) * charWidth] = bitmap.buffer[x + y * bitmap.width];

        glyphIndeces[charCode] = glyphIndex;
        metrics[charCode].horiAdvance = font->face->glyph->metrics.horiAdvance;
        metrics[charCode].horiBearingX = font->face->glyph->metrics.horiBearingX;
        metrics[charCode].horiBearingY = font->face->glyph->metrics.horiBearingY;
        metrics[charCode].width = font->face->glyph->metrics.width;
        metrics[charCode].height = font->face->glyph->metrics.height;
                
        return true;
    }
    
    void FontRenderer::GetCharacterPositions(const char* const text, size_t textLength, float * points)
    {
        float currentX = 0;
        float currentY = 0;

        bool kerning = FT_HAS_KERNING(font->face);

        for (size_t idx = 0; idx < textLength; idx++)
        {
            points[2 * idx]     = currentX + metrics[text[idx]].horiBearingX / 64.f;
            points[2 * idx + 1] = currentY - metrics[text[idx]].horiBearingY / 64.f;

            // TODO: tabification? other weird whitespace?
            if (text[idx] == '\n')
            {
                currentX = 0;
                currentY += font->face->size->metrics.height / 64; 
            }
            currentX += metrics[text[idx]].horiAdvance / 64;
            if (kerning && idx + 1 < textLength)
            {
                FT_Vector kDelta;
                FT_Get_Kerning(font->face, glyphIndeces[text[idx]], glyphIndeces[text[idx + 1]], FT_KERNING_DEFAULT, &kDelta);
                currentX += kDelta.x / 64;
            }
        }
    }

    bool FontRenderer::CreateShader(const char * const vertexSource, const char * const fragmentSource)
    {
        // TODO: cache; do not recompile shader for each one

        GLint compiled;

        unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLsizei length;
            char* buff = new char[errorLength];
            glGetShaderInfoLog(vertexShader, errorLength, &length, buff);
            SetError("Could not compile vertex shader:\n%s", buff);
            return false;
        }

        unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLsizei length;
            char* buff = new char[errorLength];
            glGetShaderInfoLog(fragmentShader, errorLength, &length, buff);
            SetError("Could not compile fragment shader:\n%s", buff);
            return false;
        }

        shader = glCreateProgram();
        glAttachShader(shader, vertexShader);
        glAttachShader(shader, fragmentShader);

        glLinkProgram(shader);

        int linked;
        glGetProgramiv(shader, GL_LINK_STATUS, &linked);
        if (!linked)
        {
            GLsizei length;
            char* buff = new char[errorLength];
            glGetProgramInfoLog(shader, errorLength, &length, buff);
            SetError("Could not link program:\n%s", buff);
            return false;
        }

        return true;
    }

    bool FontRenderer::SetShaderData()
    {
        glUseProgram(shader);

        glGenBuffers(1, &rawPositionsVbo);
        float rawPositions[8] = {
            0.f, 0.f,
            1.f, 0.f,
            0.f, 1.f,
            1.f, 1.f
        };
        glBindBuffer(GL_ARRAY_BUFFER, rawPositionsVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, rawPositions, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glUniform2f(glGetUniformLocation(shader, "charSize"), (float)charWidth, (float)charHeight);

        float sizes[512];
        for (unsigned chr = 0; chr < 256; chr++)
        {
            sizes[2 * chr] = (float)metrics[chr].width / 64;
            sizes[2 * chr + 1] = (float)metrics[chr].height / 64;
        }
        glUniform2fv(glGetUniformLocation(shader, "sizes"), 256, sizes);

        return true;
    }

    void FontRenderer::SetUniforms(float(&transform)[16], float(&colorFg)[4], float(&colorBg)[4], const char * const text, unsigned length, float* positions)
    {
        glUniformMatrix4fv(glGetUniformLocation(shader, "transform"), 1, GL_TRUE, transform);
        glUniform4fv(glGetUniformLocation(shader, "fgColor"), 1, colorFg);
        glUniform4fv(glGetUniformLocation(shader, "bgColor"), 1, colorBg);
        
        glUniform1ui(glGetUniformLocation(shader, "length"), length);
        
        unsigned characters[256]{ 0 };
        for (unsigned idx = 0; idx < 256; idx++)
            characters[idx] = (unsigned)text[idx];
        glUniform1uiv(glGetUniformLocation(shader, "characters"), 256, characters);

        float paddedPositions[512]{ 0.f };
        memcpy_s(paddedPositions, sizeof(float) * 512, positions, sizeof(float) * 2 * length);
        glUniform2fv(glGetUniformLocation(shader, "positions"), 256, paddedPositions);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, texture);
        glUniform1i(glGetUniformLocation(shader, "tex"), 0);
    }

    unsigned FontRenderer::nextPowerOfTwo(unsigned n)
    {
        unsigned result = 1;
        while (result < n)
            result *= 2;
        return result;
    }

    const char * const shaderVertexSource = R"raw(
        #version 430

        layout(location = 0) uniform mat4 transform;
        layout(location = 1) uniform uint length;
        layout(location = 2) uniform vec2 charSize;
        layout(location = 3) uniform uint characters[256];
        layout(location = 259) uniform vec2 positions[256];
        layout(location = 515) uniform vec2 sizes[256];

        layout(location = 0) in vec2 raw_position;

        layout(location = 0) out vec2 uv;
        layout(location = 1) flat out uint character;

        void main()
        {
            character = characters[gl_InstanceID];
            vec2 position = positions[gl_InstanceID];

            gl_Position = transform * vec4(position + sizes[character] * raw_position, 0.0, 1.0);
            uv = sizes[character] / charSize * raw_position;
        }
    )raw";

    const char * const shaderFragmentSource = R"raw(
        #version 430
        layout(location = 771) uniform vec4 fgColor;
        layout(location = 772) uniform vec4 bgColor;
        layout(location = 773) uniform sampler3D tex;

        layout(location = 0) in vec2 uv;
        layout(location = 1) flat in uint character;

        out vec4 color;

        void main()
        {
            vec4 texColor = texture(tex, vec3(uv, (float(character) + 0.5) / 256.0));
            color = mix(bgColor, fgColor, texColor.r);
            if (color.a == 0.0)
                discard;
        }
    )raw";
}

#undef GLSL