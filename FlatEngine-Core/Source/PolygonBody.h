#pragma once
#include "Body.h"

#include <string>


namespace FlatEngine
{
    class PolygonBody : public Body
    {
    public:
        PolygonBody(long myID = -1, long parentID = -1);
        ~PolygonBody();
        std::string GetData();
    };
}

