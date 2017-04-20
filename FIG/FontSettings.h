#pragma once
#include "EngineShared.h"
#include "FieldInitializable.h"

namespace FIG
{
    struct ENGINE_SHARED FontSettings : public FieldInitializable<FontSettings>
    {
        template<typename... TArgs>
        FontSettings(TArgs... args)
            : freetypeLoadFlags(0)
        {
            FieldInitializable::Init(args...);
            SetDefaults();
        }
        ~FontSettings();

        int freetypeLoadFlags;

        void SetDefaults();
    };
}