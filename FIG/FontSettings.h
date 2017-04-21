#pragma once
#include "EngineShared.h"
#include "FieldInitializable.h"

namespace FIG
{
    struct ENGINE_SHARED FontSettings : public FieldInitializable<FontSettings>
    {
        template<typename... TArgs>
        FontSettings(TArgs... args)
            : freetypeLoadFlags(0), faceIndex(0)
        {
            FieldInitializable::Init(args...);
            SetDefaults();
        }
        ~FontSettings();

        int freetypeLoadFlags;
        long faceIndex;

        void SetDefaults();
    };

    ADD_FIELD(int, FREETYPE_LOAD_FLAGS, FontSettings, freetypeLoadFlags);
    ADD_FIELD(long, FACE_INDEX, FontSettings, faceIndex);
}