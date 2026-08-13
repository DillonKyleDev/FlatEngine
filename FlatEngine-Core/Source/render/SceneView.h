#pragma once
#include "Types.h"
#include "components/Camera.h"
#include "components/Mesh.h"
#include "components/Transform.h"
#include "render/Line.h"
#include "tools/JsonHelper.h"
#include "tools/Pool.h"
#include "tools/Vector2.h"

#include <cstdint>
#include <string>
#include <vector>


namespace FlatEngine
{
    namespace SceneView
    {
        enum PersistentSceneObjectIndex {
            PersistentSceneObjectIndex_GridH,
            PersistentSceneObjectIndex_GridV,   
            PersistentSceneObjectIndex_XAxis,
            PersistentSceneObjectIndex_YAxis,
            PersistentSceneObjectIndex_ZAxis,
            PersistentSceneObjectIndex_Size
        };

        const int TransformGizmoID = 6;
        const int OrientationGizmoID = 7;

        enum DebugSceneObjectType {
            DebugSceneObjectType_Line,
            DebugSceneObjectType_Circle,
            DebugSceneObjectType_Quad,
            DebugSceneObjectType_Sphere,
            DebugSceneObjectType_Cube
        };

        struct SceneRenderObject {
            long ID;
            Transform transform = Transform();
            Mesh mesh = Mesh();

            void PutData(json jsonData)
            {
                ID = (long)JsonHelper::CheckJsonLong(jsonData, "id", "SceneRenderObject");	
                mesh.PutData(jsonData.at("mesh"), "SceneRenderObject");
                transform.PutData(jsonData.at("transform"), "SceneRenderObject");
            }
        };

        extern std::vector<SceneRenderObject> persistentSceneRenderObjects;
        extern std::vector<PoolObject<SceneRenderObject>> debugDrawSceneRenderObjects;
        extern UMapVector<SceneRenderObject> cameraSceneRenderObjects;		
		extern UMapVector<SceneRenderObject> lightSceneRenderObjects;
		extern SceneRenderObject transformGizmoRenderObject;
        extern SceneRenderObject orientationGizmoRenderObject;
        extern Vector2 finalImageSize;
        const uint32_t minGridStep = 1; 
        const uint32_t maxGridStep = 800;

        extern Vector2 sceneViewportCenter;
        extern Vector2 sceneViewDimensions;      
        extern Vector2 sceneViewScrolling;
        extern Vector2 sceneViewCenter;    
        extern float sceneViewGridStep;    
        extern bool b_firstSceneRenderPass;
        extern bool b_sceneHasBeenSet;
        extern bool b_sceneViewLockedOnObject;        
        extern bool b_sceneViewRightClicked;
        extern long sceneViewLockedObjectID;
        extern std::vector<Line> sceneLines;

        extern Camera sceneViewCamera;
        extern Transform sceneViewCameraTransform;

        extern void RenderSceneView(bool& b_show);         
               
        extern SceneRenderObject CreateLineObject();
        extern SceneRenderObject CreateQuadObject();
        extern SceneRenderObject CreateCircleObject();
        extern std::vector<SceneRenderObject> CreateCapsuleObject();
		extern std::vector<SceneRenderObject> CreatePolygonObject();
		extern std::vector<SceneRenderObject> CreateChainObject();
        extern void ClearDebugDrawObjects();
        extern Transform GetLineTransformForStartEndPos(Vector3 startPos, Vector3 endPos);
        extern void DebugDrawLine(Vector3 startPos, Vector3 endPos, std::string color = "debugDraw");
        extern void DebugDrawQuad(Vector3 position, Vector2 scale = Vector2(1), std::string color = "debugDraw", Vector3 rotation = Vector3());
        extern void DebugDrawCircle(Vector3 position, float radius, std::string color = "debugDraw", Vector3 rotation = Vector3());
        extern void AddDebugDrawObject(DebugSceneObjectType type, Transform transform, std::string color);
        extern void LoadSceneViewObjects();
        extern void UpdateSceneObjectColors();  
        extern long AddSceneViewCameraGizmo(Transform transform, long ownerID);      
        extern void ToggleShowSceneViewGridObjects();
        extern void SetShowSceneViewGridObjects(bool b_show);
        extern const bool ShouldShowSceneViewGridObjects();
        extern void ToggleOrthographic();
        extern void SetOrthographic(bool b_isOrthographic);
        extern const bool IsOrthoGraphic();
        extern void ToggleGridHorizontal();
        extern void SetGridHorizontal(bool b_horizontal);
        extern const bool IsGridHorizontal();
 
        extern Vector2 Scene_ConvertWorldToScreen(Vector2 positionInWorld);
        extern Vector2 Scene_ConvertScreenToWorld(Vector2 positionOnScreen);
        extern Vector2 Scene_GetMousePosWorld();
    }
}