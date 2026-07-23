#pragma once


namespace FlatEngine
{
    namespace Easing
    {
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
    }
}