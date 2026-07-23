#pragma once
#include "tools/Vector2.h"

#include "imgui.h"
#include <string>
#include <vulkan_core.h>


namespace FlatEngine
{
    class TagList;

    namespace GuiCore
    {
        extern ImGuiChildFlags childFlags;
        extern ImGuiChildFlags autoResizeChildFlags;
        extern ImGuiChildFlags resizeChildFlags;
        extern ImGuiChildFlags headerFlags;
        extern ImGuiTableFlags tableFlags;
        extern ImGuiTableFlags tableFlagsBorders;
        extern ImGuiTableFlags resizeableTableFlags;
        extern ImGuiInputTextFlags inputFlags;

        extern float childPadding;

        // Drag/Drop IDs
        extern std::string fileExplorerTarget;
        extern std::string hierarchyTarget;	

        extern std::vector<std::string> selectedFiles;

        enum CURSOR_MODE {
            CURSOR_MODE_TRANSLATE,
            CURSOR_MODE_SCALE,
            CURSOR_MODE_ROTATE,
            CURSOR_MODE_TILE_BRUSH,
            CURSOR_MODE_TILE_ERASE,
            CURSOR_MODE_TILE_COLLIDER_DRAW,
            CURSOR_MODE_TILE_MULTISELECT,
            CURSOR_MODE_TILE_MOVE,
        };
	
        extern CURSOR_MODE cursorMode;
        

        extern void SetupImGui();
        extern void RestartImGui();
        extern void QuitImGui();
        extern void SetImGuiVars();

        extern void BeginImGuiRender();
        extern void EndImGuiRender();
        extern void SetNextViewportToFillWindow();
        extern void MoveScreenCursor(float x, float y);
        extern void RenderSeparator(float topPadding, float bottomPadding, std::string separatorColor = "separator");
        extern void RenderSubTitle(std::string title);
        extern void PushScrollbarStyles();
        extern void PopScrollbarStyles();
        extern bool BeginWindow(std::string name, bool& b_isOpen, ImGuiWindowFlags windowFlags = 0, std::string bgColor = "outerWindow");
        extern void EndWindow();
        extern void BeginWindowChild(std::string title, std::string bgColor = "outerWindow", ImGuiWindowFlags flags = 0, Vector2 padding = Vector2(0));
        extern void BeginResizeWindowChild(std::string title, std::string bgColor = "outerWindow", ImGuiWindowFlags flags = 0, Vector2 padding = Vector2(0));
        extern void EndWindowChild();                
        extern void PushComboStyles();
        extern void PopComboStyles();
        extern void PushMenuStyles();
        extern void PopMenuStyles();
        extern void PushTableStyles();
        extern void PopTableStyles();
        extern bool PushTable(std::string ID, int columns, ImGuiTableFlags flags = tableFlags, Vector2 outerSize = Vector2(0), std::vector<float> widths = std::vector<float>());
        extern bool RenderFloatDragTableRow(std::string ID, std::string fieldName, float& value, float increment, float min, float max, std::string labelColor = "", std::string valueColor = "");
        extern bool RenderIntSliderTableRow(std::string ID, std::string fieldName, int& value, int increment, int min, int max, std::string color = "");
        extern bool RenderTagListTableRow(std::string ID, std::string fieldName, TagList* tagList);
        extern bool RenderIntDragTableRow(std::string ID, std::string fieldName, int& value, float speed, int min, int max);
        extern bool RenderCheckboxTableRow(std::string ID, std::string fieldName, bool& _value);
        extern void RenderSelectableTableRow(std::string ID, std::string fieldName, std::vector<std::string> options, int& currentOption);
        extern bool RenderInputTableRow(std::string ID, std::string fieldName, std::string& value, bool b_canOpenFiles = false);
        extern void RenderTextTableRow(std::string ID, std::string fieldName, std::string value, std::string value2 = "");
        extern void PopTable();
        extern bool RenderInput(std::string ID, std::string label, std::string& value, bool b_canOpenFiles = false, float inputWidth = -1, ImGuiInputTextFlags flags = 0);
        extern bool DropInput(std::string ID, std::string label, std::string displayValue, std::string dropTargetID, int& droppedValue, std::string tooltip = "", float inputWidth = -1);
        extern bool DropInputCanOpenFiles(std::string ID, std::string label, std::string displayValue, std::string dropTargetID, int& droppedValue, std::string& openedFileValue, std::string tooltip = "", float inputWidth = -1);
        extern bool RenderButton(std::string text, Vector2 size = Vector2(0), float rounding = 0, std::string color = "button", std::string hoverColor = "buttonHovered", std::string activeColor = "buttonActive", Vector2 framePadding = Vector2(5, 3));
        extern bool RenderImageButton(std::string ID, VkDescriptorSet texture, Vector2 size = Vector2(16), float rounding = 0, Vector2 padding = Vector2(1), std::string borderColor = "buttonBorder", std::string bgColor = "imageButton", std::string tint = "imageButtonTint", std::string hoverColor = "imageButtonHovered", std::string activeColor = "imageButtonActive", Vector2 uvStart = Vector2(0), Vector2 uvEnd = Vector2(1));
        extern bool RenderDragFloat(std::string text, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags = 0, std::string bgColor = "drag");
        extern bool RenderDragDouble(std::string text, float width, double& value, double increment, std::string bgColor = "drag");
        extern bool RenderDragInt(std::string text, float width, int& value, float speed, int min, int max, ImGuiSliderFlags flags = 0, std::string bgColor = "drag");
        extern bool RenderDragLong(std::string text, float width, long& value, std::string bgColor = "drag");
        extern bool RenderSliderFloat(std::string label, float& value, float increment = 0.1f, float min = 0.0f, float max = 1000, float width = -1, int digitsAfterDecimal = 3);
        extern bool RenderSliderInt(std::string label, int& value, int increment = 1, int min = 0, int max = 1000, float width = -1);
        extern void PushSliderStyles();
        extern void PopSliderStyles();
        extern bool RenderCheckbox(std::string text, bool& b_toCheck);
        extern void RenderSectionHeader(std::string headerText, float topPadding = 0.0f, float bottomPadding = 0.0f, std::string color = "sectionHeaderBg", std::string separatorColor = "sectionHeaderSeparator");
        extern bool RenderInvisibleButton(std::string ID, Vector2 startingPoint, Vector2 size, bool b_allowOverlap = true, bool b_showRect = false, ImGuiButtonFlags flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        extern bool RenderCombo(std::string ID, std::string displayedValue, std::vector<std::string> options, int& currentOption, float width = -1);
        extern bool RenderSelectable(std::string ID, std::vector<std::string> options, int& currentOption, std::string bgColor = "drag", float width = -1);
        extern bool PushTreeList(std::string ID);
        extern void RenderTreeLeaf(std::string name, std::string& nodeClicked);
        extern void PopTreeList();
        extern void RenderTextToolTip(std::string text);
        extern void BeginToolTip(std::string title);
        extern void EndToolTip();
        extern void RenderToolTipText(std::string label, std::string text);
        extern void RenderToolTipFloat(std::string label, float data);
        extern void RenderToolTipLong(std::string label, long data);
        extern void RenderToolTipLongVector(std::string label, std::vector<long> data);
    }
}