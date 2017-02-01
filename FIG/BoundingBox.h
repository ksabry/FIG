#pragma once

namespace FIG
{
    struct BoundingBox
    {
        float left;
        float right;
        float top;
        float bottom;

        inline float width()
        {
            return right - left;
        }
        inline float height()
        {
            return bottom - top;
        }
    };
}