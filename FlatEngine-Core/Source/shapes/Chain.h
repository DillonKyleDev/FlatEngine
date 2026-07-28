#pragma once
#include "shapes/Shape.h"

#include <vector>


namespace FlatEngine
{
    class Body2D;

    class Chain : public Shape
    {
    public:
        Chain(Body2D* parentBody);            

        void SetPoints(std::vector<Vector2> points);
        void UpdatePoints();
        void SetIsLoop(bool b_isLoop);
        void SetTangentSpeed(float tangentSpeed);
        void SetRollingResistance(float rollingResistance);

    private:
    };
}

