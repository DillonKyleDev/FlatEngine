#include "components/Camera.h"
#include "components/Transform.h"
#include "managers/Settings.h"
#include "render/SceneView.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"
#include "tools/Vector3.h"

#include <fstream>

namespace FL = FlatEngine;


namespace FlatEngine
{
    namespace Settings
    {
        SettingsProfile settings = SettingsProfile();

        SettingsProfile::SettingsProfile()
        {
            m_path = "";
            // Viewports
            b_showDemoWindow = false;            
            b_showMainMenuBar = true;
            b_showTileSetEditor = false;
            b_showFileExplorer = true;
            b_showSceneView = true;
            b_showGameView = true;
            b_showHierarchy = true;
            b_showPersistentScript = true;
            b_showInspector = true;
            b_showAnimator = true;
            b_showAnimationPreview = false;
            b_showKeyFrameEditor = true;
            b_showMappingContextEditor = false;
            b_showMaterialEditor = false;
            b_showLog = true;
            b_showProfiler = false;
            b_showSettings = false;
            // Log verbosity
            b_showTrace = false;
            b_showDebug = true;
            b_showInfo = false;
            b_showWarn = false;
            b_showError = true;
            b_showCritical = false;    
            // Settings
            b_fullscreen = false;
            b_vsyncEnabled = false;        
            sceneViewCameraSpeed = 80.0f;         
            fileExplorerThumbnailSize = 100.0f;            
        }

        void SettingsProfile::LoadSettings(std::string path)
        {
            m_path = path;
            json settingsJson = JsonHelper::LoadFileData(m_path);                            

            if (JsonHelper::JsonContains(settingsJson, "Settings", "Engine Settings"))
            {			
                std::string name = "Engine Settings";
                json settings = settingsJson["Settings"];
                // Viewports
                b_showDemoWindow = JsonHelper::CheckJsonBool(settings, "b_showDemoWindow", name);
                b_showMaterialEditor = JsonHelper::CheckJsonBool(settings, "b_showMaterialEditor", name);
                b_showSettings = JsonHelper::CheckJsonBool(settings, "b_showSettings", name);
                b_showSceneView = JsonHelper::CheckJsonBool(settings, "b_showSceneView", name);
                b_showGameView = JsonHelper::CheckJsonBool(settings, "b_showGameView", name);
                b_showFileExplorer = JsonHelper::CheckJsonBool(settings, "b_showFileExplorer", name);
                b_showTileSetEditor = JsonHelper::CheckJsonBool(settings, "b_showTileSetEditor", name);
                b_showHierarchy = JsonHelper::CheckJsonBool(settings, "b_showHierarchy", name);
                b_showPersistentScript = JsonHelper::CheckJsonBool(settings, "b_showPersistentScript", name);
                b_showInspector = JsonHelper::CheckJsonBool(settings, "b_showInspector", name);
                b_showAnimator = JsonHelper::CheckJsonBool(settings, "b_showAnimator", name);
                b_fullscreen = JsonHelper::CheckJsonBool(settings, "b_fullscreen", name);
                b_showAnimationPreview = JsonHelper::CheckJsonBool(settings, "b_showAnimationPreview", name);
                b_showKeyFrameEditor = JsonHelper::CheckJsonBool(settings, "b_showKeyFrameEditor", name);
                b_showLog = JsonHelper::CheckJsonBool(settings, "b_showLog", name);
                b_showProfiler = JsonHelper::CheckJsonBool(settings, "b_showProfiler", name);
                b_showMappingContextEditor = JsonHelper::CheckJsonBool(settings, "b_showMappingContextEditor", name);
                // Log Verbosity
                b_showTrace = JsonHelper::CheckJsonBool(settings, "b_showTrace", name);
                b_showDebug = JsonHelper::CheckJsonBool(settings, "b_showDebug", name);
                b_showInfo = JsonHelper::CheckJsonBool(settings, "b_showInfo", name);
                b_showWarn = JsonHelper::CheckJsonBool(settings, "b_showWarn", name);
                b_showError = JsonHelper::CheckJsonBool(settings, "b_showError", name);
                b_showCritical = JsonHelper::CheckJsonBool(settings, "b_showCritical", name);
                // Settings
                b_fullscreen = JsonHelper::CheckJsonBool(settings, "b_fullscreen", name);
                b_vsyncEnabled = JsonHelper::CheckJsonBool(settings, "b_vsyncEnabled", name);
                fileExplorerThumbnailSize = JsonHelper::CheckJsonFloat(settings, "fileExplorerThumbnailSize", name);
                SceneView::SetGridHorizontal(JsonHelper::CheckJsonBool(settings, "b_gridHorizontal", name));
                // Scene View Camera
                SceneView::sceneViewCameraTransform.SetPosition(FL::Vector3(JsonHelper::CheckJsonFloat(settings, "sceneCameraPosX", name), JsonHelper::CheckJsonFloat(settings, "sceneCameraPosY", name), JsonHelper::CheckJsonFloat(settings, "sceneCameraPosZ", name)));
                SceneView::sceneViewCamera.SetHorizontalViewAngle(JsonHelper::CheckJsonFloat(settings, "sceneCameraHorizontalViewAngle", name));
                SceneView::sceneViewCamera.SetVerticalViewAngle(JsonHelper::CheckJsonFloat(settings, "sceneCameraVerticalViewAngle", name));
                SceneView::sceneViewCamera.SetNearClippingDistance(JsonHelper::CheckJsonFloat(settings, "sceneViewNearClippingDistance", name));
                SceneView::sceneViewCamera.SetFarClippingDistance(JsonHelper::CheckJsonFloat(settings, "sceneViewFarClippingDistance", name));
                SceneView::sceneViewCamera.SetPerspectiveAngle(JsonHelper::CheckJsonFloat(settings, "sceneViewPerspectiveAngle", name));
                SceneView::sceneViewCamera.b_orthographic = (JsonHelper::CheckJsonBool(settings, "b_orthographic", name));
                SceneView::sceneViewCamera.m_orthoSize = JsonHelper::CheckJsonFloat(settings, "orthoSize", name);
                SceneView::SetShowSceneViewGridObjects(JsonHelper::CheckJsonBool(settings, "b_showGridObjects", name));
                sceneViewCameraSpeed = JsonHelper::CheckJsonFloat(settings, "sceneViewCameraSpeed", name);                
            }
            else 
            {
                Logger::log.Err("Engine settings not loaded from %{}", m_path);
            }
        }

        void SettingsProfile::SaveSettings()
        {		
            std::ofstream file_obj;
            std::ifstream ifstream(m_path);

            // Delete old file contents
            file_obj.open(m_path, std::ofstream::out | std::ofstream::trunc);
            file_obj.close();
            file_obj.open(m_path, std::ios::app);
                        
            Vector3 sceneViewPos = SceneView::sceneViewCameraTransform.GetPosition();

            json settings = json::object({
                { "path",                            m_path },
                // Viewports
                { "b_showDemoWindow",                b_showDemoWindow },     
                { "b_showFileExplorer",              b_showFileExplorer },
                { "b_showTileSetEditor",             b_showTileSetEditor },                
                { "b_showSceneView",                 b_showSceneView },
                { "b_showGameView",                  b_showGameView },
                { "b_showHierarchy",                 b_showHierarchy },
                { "b_showPersistentScript",          b_showPersistentScript },
                { "b_showInspector",                 b_showInspector },
                { "b_showAnimator",                  b_showAnimator },
                { "b_showAnimationPreview",          b_showAnimationPreview },
                { "b_showKeyFrameEditor",            b_showKeyFrameEditor },
                { "b_showLog",                       b_showLog },
                { "b_showProfiler",                  b_showProfiler },
                { "b_showMappingContextEditor",      b_showMappingContextEditor },    
                { "b_showMaterialEditor",            b_showMaterialEditor },     
                { "b_showSettings",                  b_showSettings },    
                // Log verbosity
                { "b_showTrace",                     b_showTrace },
                { "b_showDebug",                     b_showDebug },
                { "b_showInfo",                      b_showInfo },
                { "b_showWarn",                      b_showWarn },    
                { "b_showError",                     b_showError },     
                { "b_showCritical",                  b_showCritical },   
                // Settings
                { "b_fullscreen",                    b_fullscreen },
                { "b_vsyncEnabled",                  b_vsyncEnabled },
                { "fileExplorerThumbnailSize",       fileExplorerThumbnailSize },     
                { "b_gridHorizontal",                SceneView::IsGridHorizontal() },       
                // Scene View Camera
                { "sceneCameraPosX",                 sceneViewPos.x },
                { "sceneCameraPosY",                 sceneViewPos.y },
                { "sceneCameraPosZ",                 sceneViewPos.z },
                { "sceneCameraHorizontalViewAngle",  SceneView::sceneViewCamera.GetHorizontalViewAngle() },
                { "sceneCameraVerticalViewAngle",    SceneView::sceneViewCamera.GetVerticalViewAngle() },
                { "sceneViewNearClippingDistance",   SceneView::sceneViewCamera.GetNearClippingDistance() },
                { "sceneViewFarClippingDistance",    SceneView::sceneViewCamera.GetFarClippingDistance() },
                { "sceneViewPerspectiveAngle",       SceneView::sceneViewCamera.GetPerspectiveAngle() },
                { "b_orthographic",                  SceneView::sceneViewCamera.b_orthographic },
                { "orthoSize",                       SceneView::sceneViewCamera.m_orthoSize },
                { "b_showGridObjects",               SceneView::ShouldShowSceneViewGridObjects() },                
                { "sceneViewCameraSpeed",            sceneViewCameraSpeed }                
            });

            json settingsObject = json::object({ {"Settings", settings } });
            file_obj << settingsObject.dump(4).c_str() << std::endl;
            file_obj.close();
        }
    }
}