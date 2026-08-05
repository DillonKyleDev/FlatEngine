#pragma once
#include "shapes/Shape.h"


namespace FlatEngine
{
    class Body2D;

    class Polygon : public Shape
    {
    public:
        Polygon(long parentID = -1);         
        
        void SetCornerRadius(float cornerRadius);
    };
}

