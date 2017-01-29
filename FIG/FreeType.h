#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

namespace FIG
{
    class FreeType
    {
    public:
        FreeType();
        ~FreeType();

        static FreeType* GetInstance();

        FT_Library library;

        void SetError(FT_Error error);
        FT_Error Error();

        FT_Face LoadFace(const char * const filename, long index = 0);
        
    private:
        static FreeType _inst;
        FT_Error _error;
    };
}
