#pragma once


namespace FlatEngine
{
    namespace Numbers
    {
        extern int GetRandInt(int low, int high);
        extern float GetRandFloat(float low, float high);
        extern float Min(float first, float second);
        extern int Min(int first, int second);
        extern double Min(double first, double second);
        extern float Max(float first, float second);
        extern int Max(int first, int second);
        extern double Max(double first, double second);
        extern float Fmod(float number, float modWith);
        extern int Fmod(int number, int modWith);
        extern double Fmod(double number, double modWith);

        extern float Sqrt(float of);

        extern float RadiansToDegrees(float radians);
        extern float DegreesToRadians(float degrees);

        extern float Cos(float radians);
        extern float Sin(float radians);
        extern float ACos(float radians);
        extern float ASin(float radians);
        extern float CosD(float degrees);
        extern float SinD(float degrees);
        extern float ACosD(float degrees);
        extern float ASinD(float degrees);

        extern float Abs(float value);
        extern double Abs(double value);
        extern int Abs(int value);
    }
}