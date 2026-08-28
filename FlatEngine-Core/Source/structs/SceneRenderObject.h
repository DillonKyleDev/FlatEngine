#pragma once
#include "components/Transform.h"
#include "components/Mesh.h"


namespace FlatEngine
{
    struct SceneRenderObject {
        long ID = -1;
        Transform transform = Transform();
        Mesh mesh = Mesh();

        void PutData(json jsonData)
        {
            ID = (long)JsonHelper::CheckJsonLong(jsonData, "id", "SceneRenderObject");	
            mesh.PutData(jsonData.at("mesh"), "SceneRenderObject");
            transform.PutData(jsonData.at("transform"), "SceneRenderObject");
        }
    };

    extern SceneRenderObject CreateLineObject();
    extern SceneRenderObject CreateQuadObject();
    extern SceneRenderObject CreateCircleObject();
    extern std::vector<SceneRenderObject> CreateCapsuleObject();
    extern std::vector<SceneRenderObject> CreatePolygonObject();
    extern std::vector<SceneRenderObject> CreateChainObject();
}