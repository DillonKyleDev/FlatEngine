#include "structs/SceneRenderObject.h"
#include "managers/Assets.h"


namespace FlatEngine
{
    SceneRenderObject CreateLineObject()
    {
        SceneRenderObject object;
        object.mesh.CreateUniformBuffers();
        object.mesh.SetMaterial("fl_debugDraw");
        object.mesh.SetModel("../engine/models/line.obj", false);	
        object.mesh.SetUBOVec4("color", Assets::assetManager.GetColor("debug")); 				          
        object.mesh.CreateResources();
        return object;
    }
    SceneRenderObject CreateQuadObject()
    {
        SceneRenderObject object;
        object.mesh.CreateUniformBuffers();
        object.mesh.SetMaterial("fl_debugDraw");
        object.mesh.SetModel("../engine/models/quadLines.obj", false);	
        object.mesh.SetUBOVec4("color", Assets::assetManager.GetColor("debug")); 				          
        object.mesh.CreateResources();
        return object;
    }
    SceneRenderObject CreateCircleObject()
    {
        SceneRenderObject object;
        object.mesh.CreateUniformBuffers();
        object.mesh.SetMaterial("fl_debugDraw");
        object.mesh.SetModel("../engine/models/circle.obj", false);	
        object.mesh.SetUBOVec4("color", Assets::assetManager.GetColor("debug")); 				          
        object.mesh.CreateResources();
        return object;
    }
    std::vector<SceneRenderObject> CreateCapsuleObject()
    {
        std::vector<SceneRenderObject> capsuleShapes = { CreateCircleObject(), CreateCircleObject() };			
        for (int i = 0; i < 6; i++)
        {
            capsuleShapes.push_back(CreateLineObject());
        }
        return capsuleShapes;
    }
    std::vector<SceneRenderObject> CreatePolygonObject()
    {
        std::vector<SceneRenderObject> polygonShapes = { CreateLineObject(), CreateLineObject(), CreateLineObject() };			
        return polygonShapes;
    }
    std::vector<SceneRenderObject> CreateChainObject()
    {
        std::vector<SceneRenderObject> chainShapes = { CreateLineObject(), CreateLineObject(), CreateLineObject(), CreateLineObject() };			
        return chainShapes;
    }
}