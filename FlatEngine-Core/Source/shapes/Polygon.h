#pragma once
#include "shapes/Shape.h"

#include <vector>


namespace FlatEngine
{
    class Body;

    class Polygon : public Shape
    {
    public:
        Polygon(Body* parentBody = nullptr);         

        void SetPoints(std::vector<Vector2> points);
        void UpdatePoints();
        void SetCornerRadius(float cornerRadius);

    private:
    };
}

