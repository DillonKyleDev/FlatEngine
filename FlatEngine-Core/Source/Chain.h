#pragma once
#include "Shape.h"

#include <vector>


namespace FlatEngine
{
    class Body;

    class Chain : public Shape
    {
    public:
        Chain(Body* parentBody);
        ~Chain();        

        void SetEditingPoints(bool b_editingPoints);
        bool IsEditingPoints();
        void SetPoints(std::vector<Vector2> points);
        void UpdatePoints();
        void SetIsLoop(bool b_isLoop);
        void SetTangentSpeed(float tangentSpeed);
        void SetRollingResistance(float rollingResistance);

    private:
        bool m_b_editingPoints;
    };
}

