#pragma once
#include "shapes/Shape.h"


namespace FlatEngine
{
    class Body2D;

    class Polygon : public Shape
    {
    public:
        Polygon(Body2D* parentBody = nullptr);         
        
        void SetCornerRadius(float cornerRadius);
    };
}

