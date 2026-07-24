#pragma once

#include <string>


namespace FlatEngine
{
    namespace Settings
    {
        class SettingsProfile
        {
            public:
                SettingsProfile();

                void LoadSettings(std::string path);
                void SaveSettings();

                std::string m_path;
                // Viewports
                bool b_showDemoWindow;   
                bool b_showMainMenuBar;             
                bool b_showTileSetEditor;
                bool b_showFileExplorer;
                bool b_showSceneView;
                bool b_showGameView;
                bool b_showHierarchy;
                bool b_showPersistentScript;
                bool b_showInspector;
                bool b_showAnimator;
                bool b_showAnimationPreview;
                bool b_showKeyFrameEditor;
                bool b_showMappingContextEditor;
                bool b_showMaterialEditor;
                bool b_showLog;
                bool b_showProfiler;
                bool b_showSettings;
                // Log verbosity
                bool b_showTrace;
                bool b_showDebug;
                bool b_showInfo;
                bool b_showWarn;
                bool b_showError;
                bool b_showCritical;
                // Settings
                bool b_fullscreen;
                bool b_vsyncEnabled;
                float sceneViewCameraSpeed;  
                float fileExplorerThumbnailSize;

            private:        	
        };

        extern SettingsProfile settings;
    }
}