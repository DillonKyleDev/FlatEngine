#pragma once
#include "components/Camera.h"
#include "components/Mesh.h"
#include "components/Transform.h"
#include "shapes/Line.h"
#include "tools/Vector2.h"

#include <string>
#include <vector>


namespace FlatEngine
{
    namespace SceneView
    {
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

        extern Mesh gridMesh;
        extern Mesh xAxisMesh;
        extern Mesh yAxisMesh;
        extern Mesh zAxisMesh;
        extern Mesh transformGizmoMesh;
        extern Mesh orientationGizmoMesh;
        extern Mesh cameraGizmoMesh;
        extern Mesh sceneViewMeshes[7]; 
        extern Transform sceneViewTransforms[7];
        extern Camera sceneViewCamera;
        extern Transform sceneViewCameraTransform;

        extern void RenderSceneView(bool& b_show);         
               
        extern void CreateSceneViewGridObjects();
        extern void UpdateSceneObjectColors();
        extern void DeactivateTransformGizmo();
        extern void ToggleShowSceneViewGridObjects();
        extern void SetShowSceneViewGridObjects(bool b_show);
        extern const bool ShouldShowSceneViewGridObjects();
        extern void ToggleOrthographic();
        extern void SetOrthographic(bool b_isOrthographic);
        extern const bool IsOrthoGraphic();
 
        extern void AddLineToScene(Vector2 startingPoint, Vector2 endingPoint, std::string color, float thickness);
        extern void RenderSceneLines();
        extern void DrawLineInScene(Vector2 startingPoint, Vector2 endingPoint, std::string color, float thickness);
        extern Vector2 Scene_ConvertWorldToScreen(Vector2 positionInWorld);
        extern Vector2 Scene_ConvertScreenToWorld(Vector2 positionOnScreen);
        extern Vector2 Scene_GetMousePosWorld();
    }
}