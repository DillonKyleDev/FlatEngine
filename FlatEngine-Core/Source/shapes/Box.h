#pragma once
#include "shapes/Shape.h"
#include "tools/Vector2.h"


namespace FlatEngine
{
    class Body2D;

    class Box : public Shape
    {
    public:
        Box(long parentID = -1);
        
        void SetDimensions(Vector2 dimensions);
        void UpdateCorners();
        void SetCorners(Vector2 corners[4]);
        Vector2* GetCornersWorld();
        std::vector<Vector2> GetCornersScreen();
        void SetCornerRadius(float cornerRadius);

    private:
        Vector2 m_corners[4];
    };
}


