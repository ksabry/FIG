#pragma once

#include "EngineShared.h"
#include "FieldInitializable.h"

namespace FIG
{
    struct ENGINE_SHARED FontDrawSettings : public FieldInitializable<FontDrawSettings>
    {
    public:
        template<typename... TArgs>
        FontDrawSettings(TArgs... args)
            : colorFg(nullptr), colorBg(nullptr), transform(nullptr), directX(0), directY(0)
        {
            FieldInitializable::Init(args...);
        }
        ~FontDrawSettings();

        const float* colorFg;
        const float* colorBg;
        
        const float* transform;

        bool direct;

        int directX;
        int directY;

        FontDrawSettings Copy();
    };

    ADD_FIELD(const float*, COLOR_FG, FontDrawSettings, colorFg);
    ADD_FIELD(const float*, COLOR_BG, FontDrawSettings, colorBg);
    ADD_FIELD(const float*, TRANSFORM, FontDrawSettings, transform);
    ADD_FIELD(bool, DIRECT, FontDrawSettings, direct);
    ADD_FIELD(int, DIRECT_X, FontDrawSettings, directX);
    ADD_FIELD(int, DIRECT_Y, FontDrawSettings, directY);
}