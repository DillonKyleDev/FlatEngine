#pragma once
#include "Shape.h"


namespace FlatEngine
{
    class Body;

    class Box : public Shape
    {
    public:
        Box(Body* parentBody);
        ~Box();

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


