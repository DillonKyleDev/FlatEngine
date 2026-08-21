#pragma once
#include "managers/LuaManager.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

#include "imgui.h"
#include <string>
#include <vulkan_core.h>


namespace FlatEngine
{
    class TagList;

    namespace GuiCore
    {
        extern bool b_mouseDownCanWarp;
        extern Vector2 mouseDelta;
        extern Vector2 mousePos;
        extern Vector2 lastMousePos;
        const float TABLE_HEIGHT = 22.0f;

        extern ImGuiChildFlags childFlags;
        extern ImGuiChildFlags autoResizeChildFlags;
        extern ImGuiChildFlags resizeChildFlags;
        extern ImGuiChildFlags headerFlags;
        extern ImGuiTableFlags tableFlags;
        extern ImGuiTableFlags tableFlagsBorders;
        extern ImGuiTableFlags resizeableTableFlags;
        extern ImGuiInputTextFlags inputFlags;

        extern float childPadding;
        extern bool b_currentTableLight;

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
        extern void SetImGuiVars();  
        extern void QuitImGui();

        extern void WarpMouseTo(Vector2 warpTo);        
        extern void CalculateMouseDelta();

        extern void BeginImGuiRender();
        extern void EndImGuiRender();       
        extern bool BeginWindow(std::string name, bool& b_isOpen, ImGuiWindowFlags windowFlags = 0, std::string bgColor = "outerWindow");
        extern void EndWindow();
        extern void BeginWindowChild(std::string title, std::string bgColor = "outerWindow", ImGuiWindowFlags flags = 0, Vector2 padding = Vector2(0));
        extern void BeginResizeWindowChild(std::string title, std::string bgColor = "outerWindow", ImGuiWindowFlags flags = 0, Vector2 padding = Vector2(0));
        extern void EndWindowChild();                
        
        extern void SetNextViewportToFillWindow();
        extern void MoveScreenCursor(float x, float y);
        extern void RenderSectionHeader(std::string headerText, float topPadding = 0.0f, float bottomPadding = 0.0f, std::string color = "sectionHeaderBg", std::string separatorColor = "sectionHeaderSeparator", std::string textColor = "sectionHeaderText");
        extern bool BeginMainMenuBar();
        extern void EndMainMenuBar();  
        extern bool BeginMenu(const char* label, bool enabled = true);
        extern void EndMenu();
        extern bool MenuItem(const char* label, const char* shortcut = NULL, bool selected = false, bool enabled = true);
        extern bool MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true);
        extern void RenderMenuSeparator(bool b_light = true);
        extern void RenderSeparator(float topPadding, float bottomPadding, std::string separatorColor = "separator"); 
        
        // investigate later if we can remove these from EXTERN functions and just use PushStringTable() externally, etc..
        extern void PopTable();
        extern bool PushTable(std::string ID, int columns, ImGuiTableFlags flags = tableFlags, Vector2 outerSize = Vector2(0), std::vector<float> widths = std::vector<float>());

        struct TableProps {
            std::string ID;
            std::string label;
            Vector2 tableSize = Vector2(0,TABLE_HEIGHT);
            int labelWidth = 0;
            std::string labelColor;
            std::string labelTextColor = "noEditTableText";
            std::string valueColor;
            std::vector<std::string> valueLabelColors = std::vector<std::string>();            
            float floatIncrement;
            int intIncrement = 1;
            float floatMin;
            float floatMax;
            int intMin = -INT_MAX;
            int intMax = INT_MAX;
            bool b_light = true;
            bool b_lightSet = false;
            bool b_vertSeperator = true;
            ImGuiTableFlags flags = ImGuiTableFlags_RowBg;         

            TableProps(std::string setID, std::string setLabel, Vector2 setTableSize = Vector2(), float setFloatIncrement = 0.1f, float setFloatMin = -FLT_MAX, float setFloatMax = FLT_MAX, std::string setLabelColor = "noEditTableRowFieldBg", std::string setValueColor = "", int setLableWidth = 0)
            {
                ID = setID;
                label = setLabel;
                tableSize = setTableSize;
                floatIncrement = setFloatIncrement;
                floatMin = setFloatMin;
                floatMax = setFloatMax;
                labelColor = setLabelColor;
                valueColor = setValueColor;
                labelWidth = setLableWidth;
            }
        };
        extern void RenderLabelTable(TableProps tableProps);
        extern bool RenderStringTable(TableProps tableProps, std::string& value);
        extern bool RenderInt32Table(TableProps tableProps, int& value);
        extern bool RenderInt64Table(TableProps tableProps, long& value);        
        extern bool RenderFloatTable(TableProps tableProps, float& value);
        extern bool RenderDoubleTable(TableProps tableProps, double& value);
        extern bool RenderVector2Table(TableProps tableProps, Vector2& vec2);
        extern bool RenderVector3Table(TableProps tableProps, Vector3& vec3);
        extern bool RenderVector4Table(TableProps tableProps, Vector4& vec4);
        extern bool RenderBoolTable(TableProps tableProps, bool& value);
        extern void RenderTextTable(TableProps tableProps, std::vector<std::string> values);
        extern bool RenderComboTable(TableProps tableProps, std::string displayedValue, std::vector<std::string> options, int& currentOption);
        extern void RenderLuaParametersTable(std::string ID, std::string headerString, LuaManager::LuaParameterContainer& paramContainer);

        extern bool RenderInput(std::string ID, std::string label, std::string& value, bool b_canOpenFiles = false, float inputWidth = -1, ImGuiInputTextFlags flags = 0);
        extern bool DropInput(std::string ID, std::string label, std::string displayValue, std::string dropTargetID, int& droppedValue, std::string tooltip = "", float inputWidth = -1);
        extern bool DropInputCanOpenFiles(std::string ID, std::string label, std::string displayValue, std::string dropTargetID, int& droppedValue, std::string& openedFileValue, std::string tooltip = "", float inputWidth = -1);

        extern bool RenderButton(std::string text, Vector2 size = Vector2(0), float rounding = 0, std::string color = "button", std::string hoverColor = "buttonHovered", std::string activeColor = "buttonActive", Vector2 framePadding = Vector2(5, 3));
        extern bool RenderImageButton(std::string ID, VkDescriptorSet texture, Vector2 size = Vector2(16), float rounding = 0, Vector2 padding = Vector2(0), std::string borderColor = "buttonBorder", std::string bgColor = "transparent", std::string tint = "imageButtonTint", std::string hoverColor = "imageButtonHovered", std::string activeColor = "imageButtonActive", Vector2 uvStart = Vector2(0), Vector2 uvEnd = Vector2(1));
        extern bool RenderInvisibleButton(std::string ID, Vector2 startingPoint, Vector2 size, bool b_allowOverlap = true, bool b_showRect = false, ImGuiButtonFlags flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        extern bool RenderCheckbox(std::string text, bool& b_toCheck);

        extern bool RenderDragFloat(std::string ID, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags = 0, std::string bgColor = "drag");
        extern bool RenderDragDouble(std::string ID, float width, double& value, double increment, float min, float max, std::string bgColor = "drag");
        extern bool RenderDragInt(std::string ID, float width, int& value, int increment, int min, int max, ImGuiSliderFlags flags = 0, std::string bgColor = "drag");
        extern bool RenderDragLong(std::string ID, float width, long& value, int increment, int min, int max, std::string bgColor = "drag");

        extern bool RenderSliderFloat(std::string label, float& value, float increment = 0.1f, float min = 0.0f, float max = 1000, float width = -1, int digitsAfterDecimal = 3);
        extern bool RenderSliderInt(std::string label, int& value, int increment = 1, int min = 0, int max = 1000, float width = -1);
        extern void PushSliderStyles();
        extern void PopSliderStyles();

        extern void PushTreeStyles();
		extern void PopTreeStyles();

        extern void PushMenuStyles();
        extern void PopMenuStyles();

        extern void PushComboStyles();
        extern void PopComboStyles();
        extern bool RenderCombo(std::string ID, std::string displayedValue, std::vector<std::string> options, int& currentOption, float width = 0);

        extern bool RenderSelectable(std::string ID, std::vector<std::string> options, int& currentOption, std::string bgColor = "drag", float width = -1);

        extern bool PushTreeList(std::string ID);
        extern void RenderTreeLeaf(std::string name, std::string& nodeClicked);
        extern void PopTreeList();       

        extern void RenderWarningText(std::string warning);
        extern void RenderTextToolTip(std::string text);
        extern void BeginToolTip(std::string title);
        extern void EndToolTip();
        extern void RenderToolTipText(std::string label, std::string text);
        extern void RenderToolTipFloat(std::string label, float data);
        extern void RenderToolTipLong(std::string label, long data);
        extern void RenderToolTipLongVector(std::string label, std::vector<long> data);
    }
}