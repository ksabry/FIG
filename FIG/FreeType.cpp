#include "stdafx.h"
#include "FreeType.h"

namespace FIG
{
    FreeType FreeType::_inst;

    FreeType::FreeType()
    {
        FT_Error error = FT_Init_FreeType(&library);
        if (error)
            SetError(error);
    }

    FreeType::~FreeType()
    {
    }

    FreeType* FreeType::GetInstance()
    {
        return &_inst;
    }

    void FreeType::SetError(FT_Error error)
    {
        _error = error;
    }

    FT_Error FreeType::Error()
    {
        return _error;
    }

    FT_Face FreeType::LoadFace(const char * const filename, long index)
    {
        FT_Face face;
        FT_Error error = FT_New_Face(library, filename, index, &face);
        if (error)
        {
            SetError(error);
            return nullptr;
        }
        return face;
    }
}