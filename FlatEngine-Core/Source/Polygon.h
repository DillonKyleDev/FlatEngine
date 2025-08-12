#pragma once
#include "Shape.h"

#include <string>


namespace FlatEngine
{
    class Body;

    class Polygon : public Shape
    {
    public:
        Polygon(Body* parentBody);
        ~Polygon();        

        void SetPoints(std::vector<Vector2> points);
        void UpdatePoints();
        void SetCornerRadius(float cornerRadius);

    private:
    };
}

