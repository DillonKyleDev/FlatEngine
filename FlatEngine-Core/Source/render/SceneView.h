#pragma once
#include "components/Camera.h"
#include "components/Mesh.h"
#include "components/Transform.h"
#include "shapes/Line.h"
#include "tools/JsonHelper.h"
#include "tools/Pool.h"
#include "tools/Vector2.h"

#include <string>
#include <unordered_map>
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
            PersistentSceneObjectIndex_TransformGizmo,
            PersistentSceneObjectIndex_OrientationGizmo,
            PersistentSceneObjectIndex_Size
        };

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
        extern std::unordered_map<long, SceneRenderObject> transientSceneRenderObjects;

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
               
        extern void DebugDrawLine(Vector3 startPos, Vector3 endPos, std::string color = "debugDraw");
        extern void DebugDrawQuad(Vector3 position, Vector2 scale = Vector2(1), std::string color = "debugDraw", Vector3 rotation = Vector3());
        extern void DebugDrawCircle(Vector3 position, float radius, std::string color = "debugDraw", Vector3 rotation = Vector3());
        extern void AddDebugDrawObject(DebugSceneObjectType type, Transform transform, std::string color);
        extern void LoadPersistentSceneViewObjects();
        extern void UpdateSceneObjectColors();        
        extern void ToggleShowSceneViewGridObjects();
        extern void SetShowSceneViewGridObjects(bool b_show);
        extern const bool ShouldShowSceneViewGridObjects();
        extern void ToggleOrthographic();
        extern void SetOrthographic(bool b_isOrthographic);
        extern const bool IsOrthoGraphic();
        extern void ToggleGridHorizontal();
        extern void SetGridHorizontal(bool b_horizontal);
        extern const bool IsGridHorizontal();
 
        extern void AddLineToScene(Vector2 startingPoint, Vector2 endingPoint, std::string color, float thickness);
        extern void RenderSceneLines();
        extern void DrawLineInScene(Vector2 startingPoint, Vector2 endingPoint, std::string color, float thickness);
        extern Vector2 Scene_ConvertWorldToScreen(Vector2 positionInWorld);
        extern Vector2 Scene_ConvertScreenToWorld(Vector2 positionOnScreen);
        extern Vector2 Scene_GetMousePosWorld();
    }
}