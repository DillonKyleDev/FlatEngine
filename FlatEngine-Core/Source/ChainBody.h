#pragma once
#include "Body.h"

#include <vector>


namespace FlatEngine
{
    class ChainBody : public Body
    {
    public:
        ChainBody(long myID = -1, long parentID = -1);
        ~ChainBody();
        std::string GetData();

        void SetPoints(std::vector<Vector2> points);
        void UpdatePoints();
        void SetIsLoop(bool b_isLoop);
        void SetTangentSpeed(float tangentSpeed);
        void SetRollingResistance(float rollingResistance);

    private:

    };
}

