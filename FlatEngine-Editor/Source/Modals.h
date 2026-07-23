#pragma once

#include <string>


namespace FlatGui
{
    namespace Modals
    {
        extern std::string newFileName;
        extern bool b_openProjectModal;
        extern bool b_openSceneModal;
        extern bool b_openLuaModal;
        extern bool b_openCPPModal;
        extern bool b_openAnimationModal;
        extern bool b_openMappingContextModal;
        extern bool b_openMaterialModal;
        extern bool b_openTileSetModal;
        extern bool b_openPrefabModal;
        extern long gameObjectToPrefab;

        extern bool RenderInputModal(std::string label, std::string description, std::string& inputValue, bool& b_openModal);
        extern bool RenderConfirmModal(std::string label, std::string description, bool& b_openModal);
    }
}