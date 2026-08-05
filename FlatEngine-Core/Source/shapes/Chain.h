#pragma once
#include "shapes/Shape.h"


namespace FlatEngine
{
    class Body2D;

    class Chain : public Shape
    {
    public:
        Chain(long parentID);            
        void SetTangentSpeed(float tangentSpeed);
        void SetRollingResistance(float rollingResistance);

    private:
    };
}

