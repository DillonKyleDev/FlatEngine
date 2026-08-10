#include "tools/Easing.h"

#include <cmath>


namespace FlatEngine
{
    namespace Easing
    {
        float EaseInSine(float t)
        {
            return 1.0f - std::cos((t * M_PI) / 2.0f);
        }

        float EaseOutSine(float t)
        {
            return std::sin((t * M_PI) / 2.0f);
        }

        float EaseInOutSine(float t)
        {
            return -(std::cos(M_PI * t) - 1.0f) / 2.0f;
        }

        float EaseInElastic(float t)
        {
            const float C4 = (2.0f * M_PI) / 3.0f;

            return t == 0
            ? 0
            : t == 1.0f
            ? 1.0f
            : -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * C4);
        }

        float EaseOutElastic(float t)
        {
            const float C4 = (2.0f * M_PI) / 3.0f;

            return t == 0 ? 0 : ( t == 1.0f ? 1.0f : ( std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * C4) + 1.0f ) );
        }

        float EaseInOutElastic(float t)
        {
            const float C5 = (2.0f * M_PI) / 4.5f;

            return t == 0
            ? 0
            : t == 1.0f
            ? 1.0f
            : t < 0.5f
            ? -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * C5)) / 2.0f
            : (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * C5)) / 2.0f + 1.0f;
        }

        float EaseInBack(float t)
        {
            const float C1 = 1.70158f;
            const float C3 = C1 + 1.0f;

            return C3 * t * t * t - C1 * t * t;
        }

        float EaseOutBack(float t)
        {
            const float C1 = 1.70158f;
            const float C3 = C1 + 1.0f;

            return 1.0f + C3 * std::pow(t - 1.0f, 3.0f) + C1 * std::pow(t - 1.0f, 2.0f);
        }

        float EaseInOutBack(float t)
        {
            const float C1 = 1.70158f;
            const float C2 = C1 * 1.525f;

            return t < 0.5f
            ? (std::pow(2.0f * t, 2.0f) * ((C2 + 1.0f) * 2.0f * t - C2)) / 2.0f
            : (std::pow(2.0f * t - 2.0f, 2.0f) * ((C2 + 1.0f) * (t * 2.0f - 2.0f) + C2) + 2.0f) / 2.0f;
        }

        float EaseInOutQuart(float t)
        {
            return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
        }

        float EaseInOutCubic(float t)
        {
            return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
        }

        float GetT(InterpType interpType, float percentDone)
        {
            float t;

            switch (interpType)
            {
                case InterpType_Linear:           t = percentDone; break;
                case InterpType_EaseInSine:       t = Easing::EaseInSine(percentDone); break;
                case InterpType_EaseOutSine:      t = Easing::EaseOutSine(percentDone); break;
                case InterpType_EaseInOutSine:    t = Easing::EaseInOutSine(percentDone); break;
                case InterpType_EaseInElastic:    t = Easing::EaseInElastic(percentDone); break;
                case InterpType_EaseOutElastic:   t = Easing::EaseOutElastic(percentDone); break;
                case InterpType_EaseInOutElastic: t = Easing::EaseInOutElastic(percentDone); break;
                case InterpType_EaseInBack:       t = Easing::EaseInBack(percentDone); break;
                case InterpType_EaseOutBack:      t = Easing::EaseOutBack(percentDone); break;
                case InterpType_EaseInOutBack:    t = Easing::EaseInOutBack(percentDone); break;
                case InterpType_EaseInOutQuart:   t = Easing::EaseInOutQuart(percentDone); break;
                case InterpType_EaseInOutCubic:   t = Easing::EaseInOutCubic(percentDone); break;
                default: break;
            }

            return t;
        }
    }
}