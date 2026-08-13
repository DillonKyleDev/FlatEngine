#include "Logger.h"
#include "tools/Numbers.h"

#include <algorithm>
#include <cmath>
#include <random>


namespace FlatEngine
{
    namespace Numbers
    {
        int GetRandInt(int low, int high)
        {
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist(low, high);

            return dist(rng);
        }
        float GetRandFloat(float low, float high)
        {
            if (low < high)
            {
                std::random_device dev;
                std::mt19937 rng(dev());
                int multiplier = 1;

                if (low < 0)
                {
                    float chanceToBeNegative = (float)(low*(-1) / (low*(-1) + high)) * 100;			
                    std::uniform_int_distribution<std::mt19937::result_type> negativeDist(0, 100);
                    if (negativeDist(rng) <= chanceToBeNegative)
                    {
                        multiplier *= -1;
                    }

                    low = 0;
                }

                low *= 100;
                high *= 100;
                std::uniform_int_distribution<std::mt19937::result_type> dist((unsigned int)(low), (unsigned int)(high));

                return (float)(dist(rng) / 100.0f * multiplier);
            }
            else
            {
                Logger::log.Err("RandomFloat() - low must be lower than high.");
                return 0;
            }
        }

        float Min(float first, float second)
        {
            if (first < second)
            {
                return first;
            }
            else
            {
                return second;
            }
        }

        int Min(int first, int second)
        {
            if (first < second)
            {
                return first;
            }
            else
            {
                return second;
            }
        }

        double Min(double first, double second)
        {
            if (first < second)
            {
                return first;
            }
            else
            {
                return second;
            }
        }

        float Max(float first, float second)
        {
            if (first > second)
            {
                return first;
            }
            else
            {
                return second;
            }
        }

        int Max(int first, int second)
        {
            if (first > second)
            {
                return first;
            }
            else
            {
                return second;
            }
        }

        double Max(double first, double second)
        {
            if (first > second)
            {
                return first;
            }
            else
            {
                return second;
            }
        }

        float Fmod(float number, float modWith)
        {
            return (float)fmod(number, modWith);
        }

        int Fmod(int number, int modWith)
        {
            return (int)fmod(number, modWith);
        }

        double Fmod(double number, double modWith)
        {
            return fmod(number, modWith);
        }

        float Clamp(float value, float min, float max)
        {
            return std::clamp(value, min, max);
        }

        float Sqrt(float of)
        {
            return std::sqrt(of);
        }

        float RadiansToDegrees(float radians)
        {
            return radians * (180.0f / PI);
        }
        float DegreesToRadians(float degrees)
        {
            return degrees * (PI / 180.0f);
        }
        
        // (degrees)
        float Cos(float degrees)
        {
            return std::cos(DegreesToRadians(degrees));
        }        
        float Sin(float degrees)
        {
            return std::sin(DegreesToRadians(degrees));
        }
        float ACos(float x)
        {
            return RadiansToDegrees(std::acos(x));
        }        
        float ASin(float y)
        {            
            return RadiansToDegrees(std::asin(y));
        }
        float ATan(float x, float y)
        {
            return RadiansToDegrees(std::atan2(x, y));
        }
        // radians
        float CosR(float radians)
        {
            return std::cos(radians);
        }
        float SinR(float radians)
        {
            return std::sin(radians);
        }        
        float ACosR(float x)
        {
            return std::acos(x);
        }
        float ASinR(float y)
        {
            return std::asin(y);
        }
        float ATanR(float x, float y)
        {
            return std::atan2(x, y);
        }

        float Abs(float value)
        {
            if (value < 0)
            {
                value *= -1;
            }

            return value;
        }

        double Abs(double value)
        {
            if (value < 0)
            {
                value *= -1;
            }

            return value;
        }

        int Abs(int value)
        {
            if (value < 0)
            {
                value *= -1;
            }

            return value;
        }
    }
}