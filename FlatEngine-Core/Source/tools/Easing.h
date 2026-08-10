#pragma once


namespace FlatEngine
{
    namespace Easing
    {
        enum InterpType {
            InterpType_Linear,
            InterpType_EaseInSine,
            InterpType_EaseOutSine,
            InterpType_EaseInOutSine,
            InterpType_EaseInElastic,
            InterpType_EaseOutElastic,
            InterpType_EaseInOutElastic,
            InterpType_EaseInBack,
            InterpType_EaseOutBack,
            InterpType_EaseInOutBack,
            InterpType_EaseInOutQuart,
            InterpType_EaseInOutCubic
        };

        extern float EaseInSine(float t);
        extern float EaseOutSine(float t);
        extern float EaseInOutSine(float t);
        extern float EaseInElastic(float t);
        extern float EaseOutElastic(float t);
        extern float EaseInOutElastic(float t);
        extern float EaseInBack(float t);
        extern float EaseOutBack(float t);
        extern float EaseInOutBack(float t);
        extern float EaseInOutQuart(float t);
        extern float EaseInOutCubic(float t);

        extern float GetT(InterpType interpType, float percentDone);
    }
}