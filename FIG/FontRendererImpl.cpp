#include "stdafx.h"
#include "FontRendererImpl.h"
#include "FontImpl.h"
#include "FontDrawSettings.h"

#include "OpenGl.h"
#include <assert.h>
#include <iostream>

#include FT_GLYPH_H

namespace FIG
{
    FontRendererImpl::FontRendererImpl(Font* font, FontRendererSettings settings)
        : font(font), settings(settings)
    {
        if (!CreateGlyphs())
            return;
        if (!CreateShader(shaderVertexSource, shaderFragmentSource))
            return;
        if (!SetShaderData())
            return;
    }
    FontRendererImpl::~FontRendererImpl()
    {
    }

    bool FontRendererImpl::CheckGLError()
    {
        const char * err = GetGLError();
        SetError(err);
        return err != nullptr;
    }

    const float FontRendererImpl::defaultColorFg[4]{ 1.0, 1.0, 1.0, 1.0 };
    const float FontRendererImpl::defaultColorBg[4]{ 0.0, 0.0, 0.0, 0.0 };

    void FontRendererImpl::Draw(FontDrawSettings drawSettings, const char * const text)
    {
        auto newSettings = drawSettings.Copy();

        if (newSettings.transform == nullptr)
        {
            float transform[16] = {
                1,  0,  0,  0,
                0,  1,  0,  0,
                0,  0,  1,  0,
                0,  0,  0,  1
            };
            newSettings.transform = transform;
        }
        if (newSettings.colorFg == nullptr)
        {
            newSettings.colorFg = defaultColorFg;
        }
        if (newSettings.colorBg == nullptr)
        {
            float colorBg[4] = { newSettings.colorFg[0], newSettings.colorFg[1], newSettings.colorFg[2], 0.0 };
            newSettings.colorBg = colorBg;
        }

        if (drawSettings.direct)
            DrawDirect(newSettings, text);
        else
            DrawInternal(newSettings, text);
    }

    void FontRendererImpl::DrawDirect(FontDrawSettings settings, const char * const text)
    {
        int m_viewport[4];
        glGetIntegerv(GL_VIEWPORT, m_viewport);
        GetGLError();

        float left = (float)m_viewport[0] - settings.directX;
        float top = (float)m_viewport[1] - settings.directY;
        float right = (float)m_viewport[2] - settings.directX;
        float bottom = (float)m_viewport[3] - settings.directY;
        float nearVal = -1.0;
        float farVal = 1.0;

        float X = 2 / (right - left),
            Y = 2 / (top - bottom),
            Z = -2 / (farVal - nearVal),
            Tx = -(right + left) / (right - left),
            Ty = -(top + bottom) / (top - bottom),
            Tz = -(farVal + nearVal) / (farVal - nearVal);

        float transform[16] = {
            X,  0,  0,  0,
            0,  Y,  0,  0,
            0,  0,  Z,  0,
            Tx, Ty, Tz, 1
        };

        auto newSettings = settings.Copy();
        newSettings.transform = transform;

        DrawInternal(newSettings, text);
    }

    void FontRendererImpl::DrawInternal(FontDrawSettings drawSettings, const char * const text)
    {
        glBindVertexArray(0u);
        ReturnIfGLError();

        bool olBlendEnabled = glIsEnabled(GL_BLEND);

        GLint oldBlendSrc;
        GLint oldBlendDst;
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &oldBlendSrc);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &oldBlendDst);
        ReturnIfGLError();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ReturnIfGLError();

        size_t length = strlen(text);
        if (length == 0)
            return;

        float* points = new float[2 * length];
        GetCharacterPositions(text, length, points);

        glUseProgram(shader);
        ReturnIfGLError();

        glBindBuffer(GL_ARRAY_BUFFER, rawPositionsVbo);
        ReturnIfGLError();

        SetRenderData(drawSettings.transform, drawSettings.colorFg, drawSettings.colorBg, text, length, points);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, length);
        ReturnIfGLError();

        if (!olBlendEnabled)
        {
            glDisable(GL_BLEND);
            GetGLError();
        }
        
        glBlendFunc(oldBlendSrc, oldBlendDst);
        ReturnIfGLError();
    }

    BoundingBox FontRendererImpl::Bounds(const char * const text)
    {
        size_t textLength = strlen(text);

        bool kerning = settings.useKerning && FT_HAS_KERNING(font->Impl()->face);

        float currentX = 0;
        float maxLineWidth = 0;
        size_t lineCount = 0;

        size_t lineStartIdx = 0;
        for (size_t idx = 0; idx < textLength + 1; idx++)
        {
            if (text[idx] == '\n' || idx == textLength)
            {
                float lineWidth = currentX + metrics[text[idx]].horiAdvance / 64;
                maxLineWidth = lineWidth > maxLineWidth ? lineWidth : maxLineWidth;

                lineStartIdx = idx;
                currentX = 0;
                lineCount++;
            }
            if (idx != textLength)
            {
                currentX += metrics[text[idx]].horiAdvance / 64;
                if (kerning && idx + 1 < textLength)
                {
                    FT_Vector kDelta;
                    FT_Get_Kerning(font->Impl()->face, glyphIndeces[text[idx]], glyphIndeces[text[idx + 1]], FT_KERNING_DEFAULT, &kDelta);
                    currentX += kDelta.x / 64;
                }
            }
        }

        float offset = settings.alignment == TextAlignment::Left   ? 0
                     : settings.alignment == TextAlignment::Center ? maxLineWidth / 2
                     : maxLineWidth;
        
        return BoundingBox{ -offset, offset, 0, lineCount * (float)(font->Impl()->face->size->metrics.height / 64) };
    }

    BoundingBox FontRendererImpl::RenderBounds(const char * const text)
    {
        size_t length = strlen(text);
        if (length == 0)
            return BoundingBox{ 0.f, 0.f, 0.f, 0.f };

        float* points = new float[2 * length];
        GetCharacterPositions(text, length, points);

        float minX = points[0], maxX = points[0], minY = points[1], maxY = points[1];
        for (size_t i = 1; i < length; i++)
        {
            float lowX = points[2 * i], lowY = points[2 * i + 1];
            float highX = points[2 * i] + metrics[text[i]].width / 64, highY = points[2 * i + 1] + metrics[text[i]].height / 64;

            minX = lowX < minX ? lowX : minX;
            maxX = highX > maxX ? highX : maxX;
            minY = lowY < minY ? lowY : minY;
            maxY = highY > maxY ? highY : maxY;
        }
        return BoundingBox{ minX, maxX, minY, maxY };
    }

    bool FontRendererImpl::CreateGlyphs()
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_3D, texture);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        ReturnIfGLError(false);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, settings.filterMode);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, settings.filterMode);
        ReturnIfGLError(false);

        FT_F26Dot6 f266_fontsize = (long)(settings.fontSize * 64.f + 0.5f);
        FT_Set_Char_Size(font->Impl()->face, f266_fontsize, f266_fontsize, 96, 96);

        FT_Pos bboxWidth = font->Impl()->face->bbox.xMax - font->Impl()->face->bbox.xMin;
        FT_Pos bboxHeight = font->Impl()->face->bbox.yMax - font->Impl()->face->bbox.yMin;
        charWidth = NextPowerOfTwo((unsigned)ceil(font->Impl()->face->size->metrics.x_scale / 65536.0 * bboxWidth / 64.0));
        charHeight = NextPowerOfTwo((unsigned)ceil(font->Impl()->face->size->metrics.y_scale / 65536.0 * bboxHeight / 64.0));

        GLubyte* bitmapData = new GLubyte[256 * charWidth * charHeight]{ '\0' };

        FT_UInt glyphIndex;
        FT_ULong charCode = FT_Get_First_Char(font->Impl()->face, &glyphIndex);
        while (glyphIndex)
        {
            if (charCode < 256 && !CreateGlyph(charCode, glyphIndex, bitmapData))
                return false;
            charCode = FT_Get_Next_Char(font->Impl()->face, charCode, &glyphIndex);
        }

        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, charWidth, charHeight, 256, 0, GL_RED, GL_UNSIGNED_BYTE, bitmapData);
        ReturnIfGLError(false);

        return true;
    }

    bool FontRendererImpl::CreateGlyph(FT_ULong charCode, FT_UInt glyphIndex, unsigned char* bitmapData)
    {
        FT_Error ftError = FT_Load_Glyph(font->Impl()->face, glyphIndex, settings.glyphLoadFlags);
        if (ftError)
        {
            SetError("FT Error (%d) when loading glyph at index %d", ftError, glyphIndex);
            return false;
        }

        FT_Glyph glyph;
        ftError = FT_Get_Glyph(font->Impl()->face->glyph, &glyph);
        if (ftError)
        {
            SetError("FT Error (%d) when getting glyph at index %d", ftError, glyphIndex);
            return false;
        }

        FT_Glyph_To_Bitmap(&glyph, (FT_Render_Mode)settings.glyphRenderMode, 0, 1);
        FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
        FT_Bitmap& bitmap = bitmapGlyph->bitmap;

        for (unsigned y = 0; y < bitmap.rows; y++)
            for (unsigned x = 0; x < bitmap.width; x++)
                bitmapData[x + (y + charCode * charHeight) * charWidth] = bitmap.buffer[x + y * bitmap.width];

        glyphIndeces[charCode] = glyphIndex;
        metrics[charCode].horiAdvance = font->Impl()->face->glyph->metrics.horiAdvance;
        metrics[charCode].horiBearingX = font->Impl()->face->glyph->metrics.horiBearingX;
        metrics[charCode].horiBearingY = font->Impl()->face->glyph->metrics.horiBearingY;
        metrics[charCode].width = font->Impl()->face->glyph->metrics.width;
        metrics[charCode].height = font->Impl()->face->glyph->metrics.height;
                
        return true;
    }

    void FontRendererImpl::GetCharacterPositions(const char* const text, size_t textLength, float * points)
    {
        float currentX = 0;
        float currentY = 0;

        bool kerning = settings.useKerning && FT_HAS_KERNING(font->Impl()->face);

        size_t lineStartIdx = 0;

        for (size_t idx = 0; idx < textLength + 1; idx++)
        {
            if (idx != textLength)
            {
                points[2 * idx]     = currentX + metrics[text[idx]].horiBearingX / 64.f;
                points[2 * idx + 1] = currentY - metrics[text[idx]].horiBearingY / 64.f;
            }

            // TODO: tabification? other weird whitespace?
            if (text[idx] == '\n' || idx == textLength)
            {
                float lineWidth = currentX + metrics[text[idx]].horiAdvance / 64;
                float offset = settings.alignment == TextAlignment::Left   ? 0
                             : settings.alignment == TextAlignment::Center ? lineWidth / 2
                             :                                               lineWidth;
                for (size_t lineIdx = lineStartIdx; lineIdx < idx; lineIdx++)
                    points[2 * lineIdx] -= offset;

                lineStartIdx = idx;
                currentX = 0;
                currentY += font->Impl()->face->size->metrics.height / 64; 
            }
            if (idx != textLength)
            {
                currentX += metrics[text[idx]].horiAdvance / 64;
                if (kerning && idx + 1 < textLength)
                {
                    FT_Vector kDelta;
                    FT_Get_Kerning(font->Impl()->face, glyphIndeces[text[idx]], glyphIndeces[text[idx + 1]], FT_KERNING_DEFAULT, &kDelta);
                    currentX += kDelta.x / 64;
                }
            }
        }
    }

    bool FontRendererImpl::CreateShader(const char * const vertexSource, const char * const fragmentSource)
    {
        // TODO: cache; do not recompile shader for each one

        GLint compiled;

        unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
        ReturnIfGLError(false);
        
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);
        ReturnIfGLError(false);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
        ReturnIfGLError(false); 
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
        ReturnIfGLError(false);

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
        ReturnIfGLError(false);

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

    bool FontRendererImpl::SetShaderData()
    {
        glUseProgram(shader);
        ReturnIfGLError(false);

        glGenBuffers(1, &charactersVbo);
        glGenBuffers(1, &positionsVbo);
        ReturnIfGLError(false);

        glGenBuffers(1, &rawPositionsVbo);
        float rawPositions[8] = {
            0.f, 0.f,
            1.f, 0.f,
            0.f, 1.f,
            1.f, 1.f
        };
        glBindBuffer(GL_ARRAY_BUFFER, rawPositionsVbo);
        ReturnIfGLError(false);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, rawPositions, GL_STATIC_DRAW);
        ReturnIfGLError(false);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        ReturnIfGLError(false);

        glUniform2f(glGetUniformLocation(shader, "charSize"), (float)charWidth, (float)charHeight);
        ReturnIfGLError(false);

        float sizes[512];
        for (unsigned chr = 0; chr < 256; chr++)
        {
            sizes[2 * chr] = (float)metrics[chr].width / 64;
            sizes[2 * chr + 1] = (float)metrics[chr].height / 64;
        }
        glUniform2fv(glGetUniformLocation(shader, "sizes"), 256, sizes);
        ReturnIfGLError(false);

        return true;
    }

    void FontRendererImpl::SetRenderData(const float* transform, const float* colorFg, const float* colorBg, const char * const text, unsigned length, float* positions)
    {
        glUniformMatrix4fv(glGetUniformLocation(shader, "transform"), 1, GL_FALSE, transform);
        glUniform4fv(glGetUniformLocation(shader, "fgColor"), 1, colorFg);
        glUniform4fv(glGetUniformLocation(shader, "bgColor"), 1, colorBg);
        glUniform1ui(glGetUniformLocation(shader, "length"), length);
        ReturnIfGLError();

        glBindBuffer(GL_ARRAY_BUFFER, charactersVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(char) * length, text, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, 0, (void*)0);
        glVertexAttribDivisor(1, 1);
        ReturnIfGLError();

        glBindBuffer(GL_ARRAY_BUFFER, positionsVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * length * 2, positions, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glVertexAttribDivisor(2, 1);
        ReturnIfGLError();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, texture);
        glUniform1i(glGetUniformLocation(shader, "tex"), 0);
        ReturnIfGLError();
    }

    unsigned FontRendererImpl::NextPowerOfTwo(unsigned n)
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
        layout(location = 7) uniform vec2 sizes[256];

        layout(location = 0) in vec2 raw_position;
        layout(location = 1) in uint character;
        layout(location = 2) in vec2 position;

        layout(location = 0) out vec2 uv;
        layout(location = 1) flat out uint out_character;

        void main()
        {
            out_character = character;
            gl_Position = transform * vec4(position + sizes[character] * raw_position, 0.0, 1.0);
            uv = sizes[character] / charSize * raw_position;
        }
    )raw";

    const char * const shaderFragmentSource = R"raw(
        #version 430
        layout(location = 4) uniform vec4 fgColor;
        layout(location = 5) uniform vec4 bgColor;
        layout(location = 6) uniform sampler3D tex;

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