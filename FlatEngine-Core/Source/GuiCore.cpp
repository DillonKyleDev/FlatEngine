#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/LuaManager.h"
#include "render/VulkanManager.h"
#include "TagList.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "imgui_internal.h"
#include "tools/Vector2.h"
#include <imgui.h>
#include <string>


namespace FlatEngine 
{
	namespace GuiCore
	{
		// Flags		
		ImGuiChildFlags autoResizeChildFlags = ImGuiChildFlags_AutoResizeY;
		ImGuiChildFlags resizeChildFlags = ImGuiChildFlags_ResizeX | ImGuiChildFlags_AlwaysUseWindowPadding;
		ImGuiChildFlags childFlags = ImGuiChildFlags_AlwaysUseWindowPadding;
		ImGuiChildFlags headerFlags = ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize;		
		ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame;
		ImGuiTableFlags tableFlagsBorders =ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersH | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame;
		ImGuiTableFlags resizeableTableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
		ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_AutoSelectAll;
		float childPadding = 8;
		
		std::string fileExplorerTarget = "DND_FILE_PATH_OBJECT";
		std::string hierarchyTarget = "DND_HIERARCHY_OBJECT";

		std::vector<std::string> selectedFiles = std::vector<std::string>();
		CURSOR_MODE cursorMode = CURSOR_MODE::CURSOR_MODE_TRANSLATE;
		

		void SetupImGui()
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			//ImPlot::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls for imgui ui nav
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			style.WindowPadding = { 5.0f, 5.0f };
			style.DockingSeparatorSize = 1;
			style.SeparatorTextAlign = Vector2(0.5f, 0.0f);
			style.SeparatorTextBorderSize = 1;

			//ImGui_ImplSDL2_InitForSDLRenderer(F_Window->GetWindow(), F_Window->GetRenderer());
			//ImGui_ImplSDLRenderer2_Init(F_Window->GetRenderer());
			SetImGuiVars();

			Logger::log.Trace("ImGui initialized...\n");
		}

		void SetImGuiVars()
		{
			ImGuiStyle& style = ImGui::GetStyle();

			// Window and Frame
			style.Colors[ImGuiCol_WindowBg]              = Assets::assetManager.GetColor("windowBg");
			style.Colors[ImGuiCol_TitleBg]               = Assets::assetManager.GetColor("viewportTitleBg");
			style.Colors[ImGuiCol_TitleBgActive]         = Assets::assetManager.GetColor("viewportTitleBgActive");
			style.Colors[ImGuiCol_Border]  				 = Assets::assetManager.GetColor("viewportBorder");
		
			style.Colors[ImGuiCol_FrameBg]               = Assets::assetManager.GetColor("frameBg");
			style.Colors[ImGuiCol_FrameBgActive]         = Assets::assetManager.GetColor("frameBgActive");
			style.Colors[ImGuiCol_FrameBgHovered]        = Assets::assetManager.GetColor("frameBgHovered");
			style.Colors[ImGuiCol_TitleBgCollapsed]      = Assets::assetManager.GetColor("titleBgCollapsed");
			style.Colors[ImGuiCol_TextSelectedBg]        = Assets::assetManager.GetColor("textSelectedBg");
			style.Colors[ImGuiCol_PopupBg]               = Assets::assetManager.GetColor("popupBg");
			style.Colors[ImGuiCol_NavWindowingHighlight] = Assets::assetManager.GetColor("navWindowHighlight");
			style.Colors[ImGuiCol_NavHighlight]          = Assets::assetManager.GetColor("navHighlight");
			style.Colors[ImGuiCol_NavWindowingDimBg]     = Assets::assetManager.GetColor("navWindowDimBg");
			style.Colors[ImGuiCol_ModalWindowDimBg]      = Assets::assetManager.GetColor("modalWindowDimBg");		
			// Docking
			style.Colors[ImGuiCol_ResizeGripHovered]    = Assets::assetManager.GetColor("resizeGripHovered");
			style.Colors[ImGuiCol_ResizeGrip]           = Assets::assetManager.GetColor("resizeGrip");
			style.Colors[ImGuiCol_ResizeGripActive]     = Assets::assetManager.GetColor("resizeGripActive");
			style.Colors[ImGuiCol_DockingPreview]       = Assets::assetManager.GetColor("dockingPreview");
			style.Colors[ImGuiCol_DockingEmptyBg]       = Assets::assetManager.GetColor("dockingPreviewEmpty");
			// Tabs
			style.Colors[ImGuiCol_Tab] 			        = Assets::assetManager.GetColor("tab");	
			style.Colors[ImGuiCol_TabSelected]          = Assets::assetManager.GetColor("tabSelected");
			style.Colors[ImGuiCol_TabSelectedOverline]  = Assets::assetManager.GetColor("tabSelectedOverline");	
			style.Colors[ImGuiCol_TabUnfocusedActive]   = Assets::assetManager.GetColor("tabUnfocusedActive");
			style.Colors[ImGuiCol_TabActive]            = Assets::assetManager.GetColor("tabActive");	
			style.Colors[ImGuiCol_TabHovered]       	= Assets::assetManager.GetColor("tabHovered");
			style.Colors[ImGuiCol_TabUnfocused] 	    = Assets::assetManager.GetColor("tabUnfocused");	
			style.Colors[ImGuiCol_TabDimmedSelected]    = Assets::assetManager.GetColor("tabDimmedSelected");		
			// Scrollbar		
			style.Colors[ImGuiCol_ScrollbarBg]          = Assets::assetManager.GetColor("scrollbarBg");
			style.Colors[ImGuiCol_ScrollbarGrab]        = Assets::assetManager.GetColor("scrollbarGrab");
			style.Colors[ImGuiCol_ScrollbarGrabActive]  = Assets::assetManager.GetColor("scrollbarGrabActive");
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = Assets::assetManager.GetColor("scrollbarGrabHovered");			
			// Interactive
			style.Colors[ImGuiCol_SeparatorActive]      = Assets::assetManager.GetColor("buttonActive");
			style.Colors[ImGuiCol_SeparatorHovered]     = Assets::assetManager.GetColor("buttonHovered");
			style.Colors[ImGuiCol_ButtonHovered]        = Assets::assetManager.GetColor("buttonHovered");
			style.Colors[ImGuiCol_ButtonActive]         = Assets::assetManager.GetColor("buttonActive");
			style.Colors[ImGuiCol_Button]               = Assets::assetManager.GetColor("button");
			// Tables
			style.Colors[ImGuiCol_TableRowBg]           = Assets::assetManager.GetColor("tableCellDark");
			style.Colors[ImGuiCol_TableRowBgAlt]        = Assets::assetManager.GetColor("tableCellLight");
			style.Colors[ImGuiCol_TableBorderStrong]    = Assets::assetManager.GetColor("tableBorderStrong");
			style.Colors[ImGuiCol_TableBorderLight]     = Assets::assetManager.GetColor("tableBorderLight");
			// Menus
			style.Colors[ImGuiCol_Header]       		= Assets::assetManager.GetColor("treeSelectableSelected");
			style.Colors[ImGuiCol_HeaderHovered]        = Assets::assetManager.GetColor("treeSelectableHovered");
			style.Colors[ImGuiCol_HeaderActive]         = Assets::assetManager.GetColor("treeSelectableActive");
			// Modals
			style.Colors[ImGuiCol_ModalWindowDimBg] 	= Assets::assetManager.GetColor("modalWindowDimBg");
			
			// Style Vars
			style.WindowMenuButtonPosition = ImGuiDir_Right;			
			style.DisplaySafeAreaPadding = Vector2(0);
			style.TabCloseButtonMinWidthSelected = 0.0f;
			style.TabCloseButtonMinWidthUnselected = 0.0f;
			style.WindowPadding = Vector2(1);
			style.WindowBorderSize = 1.0f;				
			style.WindowRounding = 0.0f;
			style.FramePadding = Vector2(2);
			style.TabRounding = 0.0f;
			style.TabBorderSize = 0.0f;	
			style.TabBarBorderSize = 0.0f;	
			style.TabBarOverlineSize = 1.0f;
			style.ScrollbarSize = 12.0f;			
			style.DockingSeparatorSize = 1.0f;
			style.CellPadding = Vector2(0);
		}

		void RestartImGui()
		{
			//QuitImGui();
			SetupImGui();
			SetImGuiVars();
		}

		void QuitImGui()
		{
			//ImGui_ImplSDLRenderer2_Shutdown();
			//ImGui_ImplSDL2_Shutdown();
			//ImPlot::DestroyContext();
			//ImGui::DestroyContext();
		}

		void BeginImGuiRender()
		{	
			ImGui_ImplVulkan_NewFrame();		
			ImGui_ImplSDL2_NewFrame();		
			ImGui::NewFrame();
			
			ImGui::DockSpaceOverViewport();
		}
		
		void EndImGuiRender()
		{				
			ImGui::Render();
			ImDrawData* drawData = ImGui::GetDrawData();
			const bool b_isMinimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);

			if (!b_isMinimized)
			{
				VulkanManager::vulkan.DrawFrame();
			}

			ImGui::UpdatePlatformWindows(); // Only used when multi viewport support is enabled
		}

		void SetNextViewportToFillWindow()
		{
			ImGuiIO io = ImGui::GetIO();
			ImGui::SetNextWindowSize(io.DisplaySize);
			ImGui::SetNextWindowPos({ 0,0 });
		}


		// ImGui Wrappers
		void MoveScreenCursor(float x, float y)
		{
			ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + x, ImGui::GetCursorScreenPos().y + y));
		}

		void RenderSeparator(float topPadding, float bottomPadding, std::string separatorColor)
		{
			MoveScreenCursor(0, topPadding - 4);
			ImGui::PushStyleColor(ImGuiCol_Separator, Assets::assetManager.GetColor(separatorColor));
			ImGui::Separator();
			ImGui::PopStyleColor();
			MoveScreenCursor(0, bottomPadding - 3);
		}

		bool BeginWindow(std::string name, bool& b_isOpen, ImGuiWindowFlags windowFlags, std::string bgColor)
		{			
			bool b_begin = ImGui::Begin(name.c_str(), &b_isOpen, windowFlags);
			BeginWindowChild(name, bgColor);

			return b_begin;
		}

		void EndWindow()
		{
			EndWindowChild();			
			ImGui::End();
		}

		void BeginWindowChild(std::string title, std::string bgColor, ImGuiWindowFlags flags, Vector2 padding)
		{
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, Assets::assetManager.GetColor(bgColor));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding.x, padding.y));
			ImGui::BeginChild(title.c_str(), Vector2(0, 0), childFlags, flags);
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();		
		}

		void BeginResizeWindowChild(std::string title, std::string bgColor, ImGuiWindowFlags flags, Vector2 padding)
		{
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, Assets::assetManager.GetColor(bgColor));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding.x, padding.y));
			ImGui::BeginChild(title.c_str(), Vector2(0, 0), resizeChildFlags, flags);
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		void EndWindowChild()
		{
			ImGui::EndChild();
		}

		void PushComboStyles()
		{
			// ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 8));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
			ImGui::PushStyleColor(ImGuiCol_Button, Assets::assetManager.GetColor("comboArrow"));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Assets::assetManager.GetColor("comboArrowHovered"));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("comboBg"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Assets::assetManager.GetColor("comboHovered"));
			// For Selectables
			ImGui::PushStyleColor(ImGuiCol_Header, Assets::assetManager.GetColor("comboSelectable"));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, Assets::assetManager.GetColor("comboSelected"));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Assets::assetManager.GetColor("comboHighlighted"));
		}

		void PopComboStyles()
		{
			ImGui::PopStyleColor(7);
			ImGui::PopStyleVar();
		}

		void PushMenuStyles()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 4));	
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 8));		
			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, Vector2(4, 8));	
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 5));			
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, Assets::assetManager.GetColor("menuBarBg"));
			ImGui::PushStyleColor(ImGuiCol_Border, Assets::assetManager.GetColor("menuDropdownBorder"));
			ImGui::PushStyleColor(ImGuiCol_PopupBg, Assets::assetManager.GetColor("menuDropdownBg"));
			ImGui::PushStyleColor(ImGuiCol_Header, Assets::assetManager.GetColor("menuHeaderItem"));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, Assets::assetManager.GetColor("menuHeaderItemActive"));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Assets::assetManager.GetColor("menuHeaderItemHovered"));
		}

		void PopMenuStyles()
		{
			ImGui::PopStyleVar(4);
			ImGui::PopStyleColor(6);
		}

		void PushTableStyles()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));	
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);	
		}

		void PopTableStyles()
		{	
			ImGui::PopStyleVar(2);
		}

		bool PushTable(std::string ID, int columns, ImGuiTableFlags flags, Vector2 outerSize, std::vector<float> widths)
		{
			PushTableStyles();

			bool b_beginTable = ImGui::BeginTable(ID.c_str(), columns, flags, outerSize);
			
			if (b_beginTable)
			{
				for (int i = 0; i < columns; i++)
				{
					float width = 0;				
					if (widths.size() > i)
					{
						width = widths[i];
					}

					std::string columnLabel = ID + std::to_string(i);

					if (width != 0)				
						ImGui::TableSetupColumn(columnLabel.c_str(), ImGuiTableColumnFlags_WidthFixed, width);
					else				 	
						ImGui::TableSetupColumn(columnLabel.c_str(), ImGuiTableColumnFlags_WidthStretch);					
				}
			}
			else
			{
				PopTableStyles();
			}

			return b_beginTable;
		}

		bool RenderFloatDragTableRow(std::string ID, std::string fieldName, float& value, float increment, float min, float max, std::string labelColor, std::string valueColor)
		{
			ImGui::TableNextRow();			
			ImGui::TableSetColumnIndex(0);		
			if (labelColor != "")				
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(labelColor));	
			MoveScreenCursor(4, 4);
			ImGui::Text("%s", fieldName.c_str());		
			ImGui::TableSetColumnIndex(1);	
			if (valueColor != "")						
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
			bool b_isChanged = RenderDragFloat(ID.c_str(), 0, value, increment, min, max);
			ImGui::PushID(ID.c_str());
			ImGui::PopID();

			return b_isChanged;
		}

		bool RenderIntSliderTableRow(std::string ID, std::string fieldName, int& value, int increment, int min, int max, std::string color)
		{		
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			MoveScreenCursor(4, 4);
			if (color != "")
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(color));	
			ImGui::Text("%s", fieldName.c_str());			
			ImGui::TableSetColumnIndex(1);		
			PushSliderStyles();
			bool b_isChanged = RenderSliderInt(fieldName, value, increment, min, max);
			PopSliderStyles();
			ImGui::PushID(ID.c_str());
			ImGui::PopID();

			return b_isChanged;
		}

		bool RenderTagListTableRow(std::string ID, std::string fieldName, TagList* tagList)
		{
			bool b_changed = false;
			bool b_hasTag = tagList->HasTag(fieldName);
			bool b_collidesTag = tagList->CollidesTag(fieldName);
			std::string hasTagID = "##" + fieldName + "CheckboxHasTagID";
			std::string collidesTagID = "##" + fieldName + "CheckboxCollideTagID";

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			MoveScreenCursor(0, 2);
			ImGui::Text("%s", fieldName.c_str());

			ImGui::TableSetColumnIndex(1);
			MoveScreenCursor(0, 2);
			if (RenderCheckbox(hasTagID.c_str(), b_hasTag))
			{
				tagList->ToggleTag(fieldName);
				b_changed = true;
			}

			ImGui::TableSetColumnIndex(2);
			MoveScreenCursor(0, 2);
			if (RenderCheckbox(collidesTagID.c_str(), b_collidesTag))
			{
				tagList->ToggleCollides(fieldName);
				b_changed = true;
			}

			ImGui::PushID(ID.c_str());
			ImGui::PopID();

			return b_changed;
		}

		bool RenderIntDragTableRow(std::string ID, std::string fieldName, int& value, float speed, int min, int max)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			MoveScreenCursor(4, 4);
			ImGui::Text("%s", fieldName.c_str());
			ImGui::TableSetColumnIndex(1);
			ImGui::PopStyleVar();
			ImGui::SetNextItemWidth(-1);
			bool b_isChanged = RenderDragInt(ID.c_str(), 0, value, speed, min, max);
			ImGui::PushID(ID.c_str());
			ImGui::PopID();

			return b_isChanged;
		}

		bool RenderCheckboxTableRow(std::string ID, std::string fieldName, bool& b_value)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			MoveScreenCursor(0, 2);
			ImGui::Text("%s", fieldName.c_str());
			ImGui::TableSetColumnIndex(1);
			bool b_checked = RenderCheckbox("##"+ID, b_value);
			ImGui::PushID(ID.c_str());
			ImGui::PopID();

			return b_checked;
		}

		void RenderSelectableTableRow(std::string ID, std::string fieldName, std::vector<std::string> options, int& currentOption)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			MoveScreenCursor(0, 2);
			ImGui::Text("%s", fieldName.c_str());
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			ImGui::TableSetColumnIndex(1);
			ImGui::PopStyleVar();
			RenderSelectable(ID, options, currentOption);
			ImGui::PushID(ID.c_str());
			ImGui::PopID();
		}

		bool RenderInputTableRow(std::string ID, std::string fieldName, std::string& value, bool b_canOpenFiles)
		{
			bool b_edited = false;
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			MoveScreenCursor(0, 2);
			ImGui::Text("%s", fieldName.c_str());
			ImGui::TableSetColumnIndex(1);
			b_edited = RenderInput(ID, "", value, b_canOpenFiles);
			ImGui::PushID(ID.c_str());
			ImGui::PopID();

			return b_edited;
		}

		void RenderTextTableRow(std::string ID, std::string fieldName, std::string value1, std::string value2)
		{
			// Push uneditableTableTextColor text color
			ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("noEditTableText"));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			// Set table cell bg color
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(Assets::assetManager.GetColor("noEditTableRowFieldBg")));
			MoveScreenCursor(0, 2);
			ImGui::Text("%s", fieldName.c_str());
			ImGui::TableSetColumnIndex(1);
			// Set table cell bg color
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(Assets::assetManager.GetColor("noEditTableRowValueBg")));
			ImGui::Text("%s", value1.c_str());

			if (value2 != "")
			{
				ImGui::TableSetColumnIndex(2);
				// Set table cell bg color
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(Assets::assetManager.GetColor("noEditTableRowValueBg")));
				ImGui::Text("%s", value2.c_str());
			}

			ImGui::PushID(ID.c_str());
			ImGui::PopID();

			// Pop text color
			ImGui::PopStyleColor();
		}

		void PopTable()
		{
			ImGui::EndTable();
			PopTableStyles();
		}

		// Newer table functions
		void RenderLabelTable(std::string ID, std::string label, int width, bool b_light, std::string bgColor, ImGuiTableFlags flags)
		{
			Vector4 color = Assets::assetManager.GetColor(bgColor);
			if (!b_light)
				color *= 0.75f;

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(4,4));
			if (GuiCore::PushTable(ID, 1, flags, Vector2(width, 0)))			
			{						 	
				ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("noEditTableText"));
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);						
				GuiCore::MoveScreenCursor(4, 0);		
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(color));
				ImGui::Text("%s", label.c_str());			
				ImGui::PushID(ID.c_str());
				ImGui::PopID();

				ImGui::PopStyleColor();				
				GuiCore::PopTable();
			}
			ImGui::PopStyleVar();
		}

		void RenderVerticalSeparator(bool b_show)
		{
			if (b_show)
			{
				Vector2 p0 = Vector2(ImGui::GetCursorScreenPos().x - 1, ImGui::GetCursorScreenPos().y);			
				Vector2 p1 = Vector2(p0.x, p0.y + 21);
				ImGui::GetWindowDrawList()->AddLine(p0, p1, Assets::assetManager.GetColor32("tableLabelVerticalSeparator"), 1.0f);
			}
		}

		bool RenderStringTable(std::string ID, std::string label, std::string& value, Vector2 tableSize, int labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			bool b_changed = false;	

			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;

			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9 < tableSize.x / 2 ? tableSize.x / 2 : ImGui::CalcTextSize(label.c_str()).x + 10;

			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			Vector2 innerTableSize = Vector2((tableSize.x - labelWidth), tableSize.y);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + ID + "Table", 1, GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderInput(column1Label.c_str(), "", value);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			return b_changed;
		}

		bool RenderFloatTableColumns(std::string ID, std::string label, float& value, std::string labelColor, std::string valueColor, int labelWidth, int labelIndex, int valueIndex, float increment, float min, float max)
		{								
			float textWidth = ImGui::CalcTextSize(label.c_str()).x;
			float offsetX = (labelWidth - textWidth) * 0.5f;

			ImGui::TableSetColumnIndex(labelIndex);		
			if (labelColor != "")				
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(labelColor));	
			ImGui::AlignTextToFramePadding();	
			if (offsetX > 0.0f)		
				GuiCore::MoveScreenCursor(offsetX, 0);
			ImGui::Text("%s", label.c_str());					
			ImGui::TableSetColumnIndex(valueIndex);	
			if (valueColor != "")						
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
			return GuiCore::RenderDragFloat(ID.c_str(), 0, value, increment, min, max);
		}

		bool RenderVector2Table(std::string ID, std::string label, Vector2& vec2, Vector2 tableSize, int labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light, float increment, float min, float max, bool b_vertSeperator)
		{						
			bool b_changed = false;				

			if (valueLabelColors.size() == 0)
				valueLabelColors = { "transformXBGLight", "transformYBGLight" };
			
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
			std::string column2Label = ID + std::to_string(2);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 10;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableSize = Vector2((tableSize.x - labelWidth), tableSize.y);

			std::vector<float> widths { 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
						
			if (GuiCore::PushTable("##" + ID + "Table", 4, GuiCore::tableFlags, tableSize, widths))
			{
				ImGui::TableNextRow();			
				b_changed |= RenderFloatTableColumns(column1Label, "X", vec2.x, valueLabelColors[0], valueColor, 16, 0, 1, increment, min, max);
				b_changed |= RenderFloatTableColumns(column2Label, "Y", vec2.y, valueLabelColors[1], valueColor, 16, 2, 3, increment, min, max);
				GuiCore::PopTable();
			}
			
			FL::GuiCore::MoveScreenCursor(0, -4);
			return b_changed;
		}

		bool RenderVector3Table(std::string ID, std::string label, Vector3& vec3, Vector2 tableSize, int labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light, float increment, float min, float max, bool b_vertSeperator)
		{				
			bool b_changed = false;	

			if (valueLabelColors.size() == 0)
				valueLabelColors = { "transformXBGLight", "transformYBGLight", "transformZBGLight" };

			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
			std::string column2Label = ID + std::to_string(2);
			std::string column3Label = ID + std::to_string(3);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 10;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableSize = Vector2((tableSize.x - labelWidth), tableSize.y);

			std::vector<float> widths { 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			
			if (GuiCore::PushTable("##" + ID + "Table", 6, GuiCore::tableFlags, tableSize, widths))
			{
				ImGui::TableNextRow();					
				b_changed |= RenderFloatTableColumns(column1Label, "X", vec3.x, valueLabelColors[0], valueColor, 16, 0, 1, increment, min, max);
				b_changed |= RenderFloatTableColumns(column2Label, "Y", vec3.y, valueLabelColors[1], valueColor, 16, 2, 3, increment, min, max);
				b_changed |= RenderFloatTableColumns(column3Label, "Z", vec3.z, valueLabelColors[2], valueColor, 16, 4, 5, increment, min, max);
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);
			return b_changed;
		}

		bool RenderVector4Table(std::string ID, std::string label, Vector4& vec4, Vector2 tableSize, int labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light, float increment, float min, float max, bool b_vertSeperator)
		{			
			bool b_changed = false;	

			if (valueLabelColors.size() == 0)
				valueLabelColors = { "transformXBGLight", "transformYBGLight", "transformZBGLight", "transformWBGLight" };

			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
			std::string column2Label = ID + std::to_string(2);
			std::string column3Label = ID + std::to_string(3);
			std::string column4Label = ID + std::to_string(4);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 10;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);
			
			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableSize = Vector2((tableSize.x - labelWidth), tableSize.y);

			std::vector<float> widths { 16, 0, 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";
			
			if (GuiCore::PushTable("##" + ID + "Table", 8, GuiCore::tableFlags, tableSize, widths))
			{
				ImGui::TableNextRow();			
				b_changed |= RenderFloatTableColumns(column1Label, "X", vec4.x, valueLabelColors[0], valueColor, 16, 0, 1, increment, min, max);
				b_changed |= RenderFloatTableColumns(column2Label, "Y", vec4.y, valueLabelColors[1], valueColor, 16, 2, 3, increment, min, max);
				b_changed |= RenderFloatTableColumns(column3Label, "Z", vec4.z, valueLabelColors[2], valueColor, 16, 4, 5, increment, min, max);
				b_changed |= RenderFloatTableColumns(column4Label, "W", vec4.w, valueLabelColors[3], valueColor, 16, 6, 7, increment, min, max);
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);
			return b_changed;
		}
		
		bool RenderFloatTable(std::string ID, std::string label, float& value, Vector2 tableSize, int labelWidth, std::string labelColor, bool b_light, float increment, float min, float max, bool b_vertSeperator)
		{
			bool b_changed = false;
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;

			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9 < tableSize.x / 2.0f ? tableSize.x / 2.0f : ImGui::CalcTextSize(label.c_str()).x + 10;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableSize = Vector2((tableSize.x - labelWidth), tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + ID + "Table", 1, GuiCore::tableFlags, tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderDragFloat(column1Label.c_str(), 0, value, increment, min, max);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);
			return b_changed;
		}

		bool RenderDoubleTable(std::string ID, std::string label, double& value, Vector2 tableSize, int labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			bool b_changed = false;
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;

			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9 < tableSize.x / 2 ? tableSize.x / 2 : ImGui::CalcTextSize(label.c_str()).x + 10;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableSize = Vector2((tableSize.x - labelWidth), tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + ID + "Table", 1, GuiCore::tableFlags, tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderDragDouble(column1Label.c_str(), 0, value, 0.1f);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);
			return b_changed;
		}

		bool RenderInt32Table(std::string ID, std::string label, int& value, Vector2 tableSize, int labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			bool b_changed = false;
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;

			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9 < tableSize.x / 2 ? tableSize.x / 2 : ImGui::CalcTextSize(label.c_str()).x + 10;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableSize = Vector2((tableSize.x - labelWidth), tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + ID + "Table", 1, GuiCore::tableFlags, tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderDragInt(column1Label.c_str(), 0, value, 1, -INT_MAX, INT_MAX);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);
			return b_changed;
		}

		bool RenderInt64Table(std::string ID, std::string label, long& value, Vector2 tableSize, int labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			bool b_changed = false;
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;

			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9 < tableSize.x / 2 ? tableSize.x / 2 : ImGui::CalcTextSize(label.c_str()).x + 10;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableSize = Vector2((tableSize.x - labelWidth), tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + ID + "Table", 1, GuiCore::tableFlags, tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderDragLong(column1Label.c_str(), 0, value);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);
			return b_changed;
		}

		bool RenderBoolTable(std::string ID, std::string label, bool& value, Vector2 tableSize, int labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			bool b_changed = false;
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;

			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9 < tableSize.x / 2 ? tableSize.x / 2 : ImGui::CalcTextSize(label.c_str()).x + 10;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableSize = Vector2((tableSize.x - labelWidth), tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			std::vector<std::string> trueFalse = { "false", "true" };
			int currentBool = value ? 1 : 0;
			
			if (GuiCore::PushTable("##" + ID + "Table", 1, GuiCore::tableFlags, tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				
				b_changed = GuiCore::RenderCombo(column1Label.c_str(), value ? "true" : "false", trueFalse, currentBool, tableSize.x);
				
				if (b_changed)
					value = (bool)currentBool;	

				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);
			return b_changed;
		}

		void RenderTextTable(std::string ID, std::string fieldName, std::string value, int labelWidth, bool b_light)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(5, 4));	
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);	
			ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
			float tableWidth = ImGui::GetContentRegionAvail().x;

			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(fieldName.c_str()).x + 9 < tableWidth / 2 ? tableWidth / 2 : ImGui::CalcTextSize(fieldName.c_str()).x + 10;
			RenderLabelTable("##labelTable" + ID, fieldName, labelWidth, b_light, "noEditTableRowValueBg", flags);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(true);

			// std::vector<float> widths { 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";				
			if (FL::GuiCore::PushTable(ID, 1, flags))
			{								
				ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("noEditTableText"));
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);				
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(Assets::assetManager.GetColor(valueColor)));
				MoveScreenCursor(4,0);
				ImGui::Text("%s", value.c_str());
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				ImGui::PopStyleColor();			
				FL::GuiCore::PopTable();
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

			FL::GuiCore::MoveScreenCursor(0, -4);
		}

		bool RenderComboTable(std::string ID, std::string label, std::string displayedValue, std::vector<std::string> options, int& currentOption, Vector2 tableSize, int labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			bool b_changed = false;
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (tableSize.x == 0)
				tableSize.x = ImGui::GetContentRegionAvail().x;

			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9 < tableSize.x / 2 ? tableSize.x / 2 : ImGui::CalcTextSize(label.c_str()).x + 10;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			Vector2 innerTableSize = Vector2((tableSize.x - labelWidth), tableSize.y);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			

			if (GuiCore::PushTable("##" + ID + "Table", 1, GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				
				b_changed = GuiCore::RenderCombo(column1Label.c_str(), displayedValue, options, currentOption, innerTableSize.x);

				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);
			return b_changed;
		}

		void RenderLuaParamtersTable(std::string ID, std::string headerString, LuaManager::LuaParameterContainer& paramContainer)
		{
			// Set the values for a new parameter							
			std::vector<std::string> types = { "-none-", "string", "int32", "int64", "float", "double", "bool", "vec2", "vec3", "vec4" };
			LuaManager::LuaParameter newParam = LuaManager::LuaParameter();					
			
			std::string newParamComboID = "##ParameterType-createNewLuaScript_" + ID;
			if (GuiCore::RenderCombo(newParamComboID, types[paramContainer.tempParameterType], types, paramContainer.tempParameterType, 80))
			{
				newParam.type = (LuaManager::ParameterType)paramContainer.tempParameterType;
			}

			ImGui::SameLine(0,3);
			GuiCore::RenderInput("##newScriptParamInputLuaScript_" + ID, "", paramContainer.tempParameterName, false, ImGui::GetContentRegionAvail().x - 38);
			
			ImGui::SameLine(0,3);
			ImGui::BeginDisabled(paramContainer.tempParameterName == "" || paramContainer.tempParameterType == LuaManager::ParameterType_None);
			if (GuiCore::RenderButton("ADD##LuaScript_" + ID, Vector2(35, 21)))
			{
				newParam.type = (LuaManager::ParameterType)paramContainer.tempParameterType;
				newParam.name = paramContainer.tempParameterName;
				paramContainer.Add(newParam);
				paramContainer.tempParameterName = "";
				paramContainer.tempParameterType = 0;
			}
			ImGui::EndDisabled();		

			if (paramContainer.parameters.size())
				GuiCore::RenderSectionHeader(headerString, 3.0f, 4.0f, "sectionHeaderSecondaryBg");				

			int paramCounter = 0;
			std::string paramQueuedForDelete = "";
			for (auto paramIter = paramContainer.parameters.begin(); paramIter != paramContainer.parameters.end(); paramIter++)
			{
				LuaManager::LuaParameter& param = paramIter->second;
				int currentParamType = param.type;
				
				std::string currentParamComboID = "##ParameterTypeLuaScript_" + ID + std::to_string(paramCounter);
				if (GuiCore::RenderCombo(currentParamComboID, types[currentParamType], types, currentParamType, 80))
				{
					param.type = (LuaManager::ParameterType)currentParamType;
				}

				ImGui::SameLine(0,3);
				int trashButtonWidth = 23;								
				Vector2 tableSize = ImGui::GetContentRegionAvail().x - trashButtonWidth;
				tableSize.y = 0.0f;						
				std::string inputElementID = "LuaScript_" + ID + std::to_string(paramCounter);		
				switch(param.type)
				{
					case LuaManager::ParameterType_String: GuiCore::RenderStringTable("##String" + inputElementID, param.name, param.p_string, tableSize); break;
					case LuaManager::ParameterType_Int32:  GuiCore::RenderInt32Table("##Int32" + inputElementID, param.name, param.p_int32, tableSize); break;
					case LuaManager::ParameterType_Int64:  GuiCore::RenderInt64Table("##Int64" + inputElementID, param.name, param.p_int64, tableSize); break;
					case LuaManager::ParameterType_Float:  GuiCore::RenderFloatTable("##Float" + inputElementID, param.name, param.p_float, tableSize); break;
					case LuaManager::ParameterType_Double: GuiCore::RenderDoubleTable("##Double" + inputElementID, param.name, param.p_double, tableSize); break;
					case LuaManager::ParameterType_Bool:   GuiCore::RenderBoolTable("##Bool" + inputElementID, param.name, param.p_bool, tableSize); break;
					case LuaManager::ParameterType_Vec2:   GuiCore::RenderVector2Table("##Vector2" + inputElementID, param.name, param.p_vec2, tableSize); break;															
					case LuaManager::ParameterType_Vec3:   GuiCore::RenderVector3Table("##Vector3" + inputElementID, param.name, param.p_vec3, tableSize); break;													
					case LuaManager::ParameterType_Vec4:   GuiCore::RenderVector4Table("##Vector4" + inputElementID, param.name, param.p_vec4, tableSize); break;										
					default: break;
				}

				ImGui::SameLine(0, 2);

				std::string trashcanID = "##trashIcon-LuaScript_" + ID + std::to_string(paramCounter);
				if (GuiCore::RenderImageButton(trashcanID.c_str(), Assets::assetManager.GetTexture("trash"), Vector2(15), 0.0f, Vector2(3)))
				{
					paramQueuedForDelete = paramIter->first;
				}

				paramCounter++;
			}

			GuiCore::RenderSeparator(9, 9);

			if (paramQueuedForDelete != "")
			{
				paramContainer.Remove(paramQueuedForDelete);
				paramQueuedForDelete = "";
			}
		}







		bool RenderInput(std::string ID, std::string label, std::string& value, bool b_canOpenFiles, float inputWidth, ImGuiInputTextFlags flags)
		{
			bool b_editedButton = false;
			bool b_editedInput = false;
			bool b_dragTargeted = false;
			char newPath[1024] = {};

			#ifdef _WINDOWS
				strcpy_s(newPath, value.c_str());
			#elif _LINUX
				strcpy(newPath, value.c_str());
			#endif

			std::string pathString = label;
			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5,3));

			if (pathString != "")
			{
				pathString += ":";
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(pathString.c_str());
				ImGui::SameLine(0, 5);			
			}

			if (b_canOpenFiles && inputWidth == -1)
			{
				inputWidth = ImGui::GetContentRegionAvail().x - 23;
			}
			else if (b_canOpenFiles)
			{
				inputWidth -= 23;
			}
			else if (inputWidth == -1)
			{
				inputWidth = ImGui::GetContentRegionAvail().x;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
			Vector2 inputStart = ImGui::GetCursorScreenPos();
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("input"));
			ImGui::SetNextItemWidth(inputWidth);
			b_editedInput = ImGui::InputText(ID.c_str(), newPath, IM_ARRAYSIZE(newPath), flags);
			ImGui::PopStyleColor();
			Vector2 inputSize = Vector2(inputWidth, ImGui::GetCursorScreenPos().y - inputStart.y);
			ImGui::PopStyleVar(2);

			if (b_canOpenFiles)
			{
				ImGui::SameLine();

				std::string buttonId = ID + "openFileButton";
				if (RenderImageButton(buttonId.c_str(), Assets::assetManager.GetTexture("openFile"), Vector2(15), 1, Vector2(3), "buttonBorder", "openFileButtonBg", "imageButtonTint", "openFileButtonHovered", "imageButtonActive"))
				{
					std::string assetPath = FileHelper::OpenLoadFileExplorer();				

					#ifdef _WINDOWS
						strcpy_s(newPath, assetPath.c_str());
					#elif _LINUX
						strcpy(newPath, assetPath.c_str());
					#endif

					b_editedButton = true;
				}
			}

			ImGui::PopStyleVar();

			if (newPath[0] != '\0')
			{
				value = newPath;
			}
			return b_editedButton || b_editedInput || b_dragTargeted;
		}

		bool DropInput(std::string ID, std::string label, std::string displayValue, std::string dropTargetID, int& droppedValue, std::string toolTip, float inputWidth)
		{		
			bool b_dragTargeted = false;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 3));

			if (label != "")
			{
				label += ":";
				ImGui::AlignTextToFramePadding();
				ImGui::Text("%s", label.c_str());
				ImGui::SameLine(0, 5);			
			}

			if (inputWidth == -1)
			{
				inputWidth = ImGui::GetContentRegionAvail().x;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
			Vector2 inputStart = ImGui::GetCursorScreenPos();
			Vector2 inputSize = Vector2(inputWidth, ImGui::GetFontSize() * 1.65f);
			ImGui::GetWindowDrawList()->AddRectFilled(inputStart, Vector2(inputStart.x + inputSize.x, inputStart.y + inputSize.y), Assets::assetManager.GetColor32("input"), 0);
			ImGui::SetCursorScreenPos(Vector2(inputStart.x + 3, inputStart.y + 1));
			ImGui::Text("%s", displayValue.c_str());

			RenderInvisibleButton("##DropInputdropTarget", inputStart, inputSize, true, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
			ImGui::PopStyleVar();
			if (toolTip != "" && ImGui::IsItemHovered())
			{
				RenderTextToolTip(toolTip);
			}

			// Drop Target
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dropTargetID.c_str()))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					droppedValue = *(const int*)payload->Data;
					b_dragTargeted = true;
				}
				ImGui::EndDragDropTarget();
			}

			// Right click menu
			if (ImGui::BeginPopupContextItem("##DropInputdropTarget"))
			{
				PushMenuStyles();
				if (ImGui::MenuItem("Remove reference"))
				{
					droppedValue = -1;
					b_dragTargeted = true;
					ImGui::CloseCurrentPopup();
				}
				PopMenuStyles();

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			return b_dragTargeted;
		}

		bool DropInputCanOpenFiles(std::string ID, std::string label, std::string displayValue, std::string dropTargetID, int& droppedValue, std::string& openedFileValue, std::string toolTip, float inputWidth)
		{
			bool b_editedButton = false;
			bool b_dragTargeted = false;
			char newPath[1024];		

			#ifdef _WINDOWS
				strcpy_s(newPath, openedFileValue.c_str());
			#elif _LINUX
				strcpy(newPath, openedFileValue.c_str());
			#endif

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
			ImGui::AlignTextToFramePadding();

			if (label != "")
			{
				label += ":";
				ImGui::Text("%s", label.c_str());
				ImGui::SameLine(0, 5);			
			}

			if (inputWidth == -1)
			{
				inputWidth = ImGui::GetContentRegionAvail().x - 24;
			}
			else
			{
				inputWidth -= 24;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
			Vector2 inputStart = ImGui::GetCursorScreenPos();
			Vector2 inputSize = Vector2(inputWidth, ImGui::GetFontSize() * 1.65f);
			ImGui::GetWindowDrawList()->AddRectFilled(inputStart, Vector2(inputStart.x + inputSize.x, inputStart.y + inputSize.y), Assets::assetManager.GetColor32("input"), 0);
			ImGui::SetCursorScreenPos(Vector2(inputStart.x + 3, inputStart.y));
			ImGui::Text("%s", displayValue.c_str());

			RenderInvisibleButton("##DropInputOpenFilesdropTarget" + ID, inputStart, inputSize, true, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
			ImGui::PopStyleVar();
			if (toolTip != "" && ImGui::IsItemHovered())
			{
				RenderTextToolTip(toolTip);
			}

			// Drop Target
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dropTargetID.c_str()))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					droppedValue = *(const int*)payload->Data;
					b_dragTargeted = true;
				}
				ImGui::EndDragDropTarget();
			}

			// Right click menu
			std::string dropTargetRightClickID = "##DropInputOpenFilesdropTarget##" + ID;
			if (ImGui::BeginPopupContextItem(dropTargetRightClickID.c_str()))
			{
				PushMenuStyles();
				if (ImGui::MenuItem("Remove reference"))
				{
					droppedValue = -2;
					b_dragTargeted = true;
					ImGui::CloseCurrentPopup();
				}
				PopMenuStyles();

				ImGui::EndPopup();
			}

			ImGui::SameLine(0,3);

			std::string buttonId = ID + "openFileButton";		
			if (RenderImageButton(buttonId.c_str(), Assets::assetManager.GetTexture("openFile"), Vector2(15), 1, Vector2(3), "buttonBorder", "openFileButtonBg", "imageButtonTint", "openFileButtonHovered", "imageButtonActive"))
			{
				std::string assetPath = FileHelper::OpenLoadFileExplorer();		

				#ifdef _WINDOWS
					strcpy_s(newPath, assetPath.c_str());
				#elif _LINUX
					strcpy(newPath, assetPath.c_str());
				#endif

				b_editedButton = true;
			}

			if (newPath[0] != '\0')
			{
				openedFileValue = newPath;
			}

			ImGui::PopStyleVar();

			return b_editedButton || b_dragTargeted;
		}

		bool RenderCombo(std::string ID, std::string displayedValue, std::vector<std::string> options, int& currentOption, float width)
		{
			bool b_interactedWith = false;

			if (width != 0)
			{
				ImGui::SetNextItemWidth(width);
			}
			else 
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			}

			PushComboStyles();
			if (ImGui::BeginCombo(ID.c_str(), options[currentOption].c_str()))
			{
				for (int i = 0; i < options.size(); i++)
				{
					bool b_isSelected = (options[currentOption] == options[i]);
					if (ImGui::Selectable(options[i].c_str(), b_isSelected))
					{
						currentOption = i;
						b_interactedWith = true;
					}
				}
				ImGui::EndCombo();
			}
			PopComboStyles();

			return b_interactedWith;
		}

		bool RenderSelectable(std::string ID, std::vector<std::string> options, int& currentOption, std::string bgColor, float width)
		{
			bool b_selectionMade = false;
			bool b_currentSelectionEmpty = false;
			std::string empty = " - empty -";
			std::string currentlySelected = empty;

			if (options.size() == 0)
			{
				options.push_back(empty);
			}
			if (options.size() <= currentOption)
			{
				currentOption = 0;
			}

			PushComboStyles();
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor(bgColor));
			ImGui::SetNextItemWidth(width != -1 ? width : ImGui::GetContentRegionAvail().x);

			currentlySelected = " " + options[currentOption];

			if (options[currentOption] == "")
			{
				currentlySelected = empty;
				ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("logText"));
				b_currentSelectionEmpty = true;
			}

			if (ImGui::BeginCombo(ID.c_str(), currentlySelected.c_str()))
			{
				for (int i = 0; i < options.size(); i++)
				{
					bool b_isSelected = (options[currentOption] == options[i]);
					
					std::string selectableLabel = " " + options[i];
					if (options[i] == "")
					{
						selectableLabel = empty;
					}
						
					if (options[i] == "")
					{
						ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("logText"));
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("white"));
					}

					if (ImGui::Selectable(selectableLabel.c_str(), b_isSelected))
					{
						currentOption = i;
						b_selectionMade = true;
					}
					if (b_isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}

					ImGui::PopStyleColor();
				}
				ImGui::EndCombo();
			}

			if (b_currentSelectionEmpty)
			{
				ImGui::PopStyleColor();
			}

			PopComboStyles();
			ImGui::PopStyleColor();

			return b_selectionMade;
		}

		bool PushTreeList(std::string ID)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("innerWindow"));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			PushMenuStyles();
			bool b_beginTable = ImGui::BeginTable(ID.c_str(), 1, tableFlags);

			if (b_beginTable)
			{
				ImGui::TableSetupColumn("##PROPERTY", 0, ImGui::GetContentRegionAvail().x + 1);
			}
			else
			{
				PopMenuStyles();
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			}

			return b_beginTable;
		}

		void PopTreeList()
		{
			ImGui::EndTable();
			PopMenuStyles();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		void RenderTreeLeaf(std::string name, std::string& nodeClicked)
		{
			ImGuiTreeNodeFlags nodeFlags;

			std::string treeID = name + "_node";
			if (nodeClicked == name)
			{
				nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			}
			else
			{
				nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			}

			//// TreeNode Opener - No TreePop because it's a leaf
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, "%s", name.c_str());
			if (ImGui::IsItemClicked())
			{
				nodeClicked = name;
			}

			ImGui::PushID(treeID.c_str());
			ImGui::PopID();
		}

		bool RenderButton(std::string text, Vector2 size, float rounding, std::string color, std::string hoverColor, std::string activeColor, Vector2 framePadding)
		{
			bool b_isClicked;

			ImGui::PushStyleColor(ImGuiCol_Button, Assets::assetManager.GetColor(color));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Assets::assetManager.GetColor(hoverColor));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, Assets::assetManager.GetColor(activeColor));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, framePadding);

			if (size.x != 0 || size.y != 0)
			{
				b_isClicked = ImGui::Button(text.c_str(), size);
			}
			else
			{
				b_isClicked = ImGui::Button(text.c_str());
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
			}
		
			ImGui::PopStyleVar(2);		
			ImGui::PopStyleColor(3);		

			return b_isClicked;
		}

		bool RenderImageButton(std::string ID, VkDescriptorSet texture, Vector2 size, float rounding, Vector2 padding, std::string borderColor, std::string bgColor, std::string tint, std::string hoverColor, std::string activeColor, Vector2 uvStart, Vector2 uvEnd)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, Assets::assetManager.GetColor(bgColor));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Assets::assetManager.GetColor(hoverColor));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, Assets::assetManager.GetColor(activeColor));		
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
			
			Vector2 startPos = ImGui::GetCursorScreenPos();
			Vector2 fullSize = Vector2((size.x + (padding.x * 2)), (size.y + (padding.y * 2)));
			bool b_isClicked = ImGui::ImageButton(ID.c_str(), texture, size, uvStart, uvEnd, Assets::assetManager.GetColor("transparent"), Assets::assetManager.GetColor(tint));
			ImGui::GetWindowDrawList()->AddRect(startPos, startPos + fullSize, Assets::assetManager.GetColor32(borderColor), 0, 0, 1.0f);

			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
			}

			ImGui::PopStyleVar(2);	
			ImGui::PopStyleColor(3);

			return b_isClicked;
		}

		bool RenderDragFloat(std::string ID, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags, std::string bgColor)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor(bgColor));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Assets::assetManager.GetColor("dragHovered"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, Assets::assetManager.GetColor("dragActive"));

			if (width != 0)
			{
				ImGui::SetNextItemWidth(width);
			}
			else
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			}

			bool b_sliderChanged = ImGui::DragFloat(ID.c_str(), &value, increment, min, max, "%.3f", flags | ImGuiSliderFlags_AlwaysClamp);

			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
			}

			ImGui::PopStyleColor(3);

			return b_sliderChanged;
		}

		bool RenderDragDouble(std::string ID, float width, double& value, double increment, std::string bgColor)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor(bgColor));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Assets::assetManager.GetColor("dragHovered"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, Assets::assetManager.GetColor("dragActive"));

			if (width != 0)
			{
				ImGui::SetNextItemWidth(width);
			}
			else
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			}

			bool b_sliderChanged = ImGui::DragScalar(ID.c_str(), ImGuiDataType_Double, &value, increment, "%.3f", NULL, NULL, ImGuiSliderFlags_AlwaysClamp);			

			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
			}

			ImGui::PopStyleColor(3);

			return b_sliderChanged;
		}

		bool RenderDragInt(std::string ID, float width, int& value, float speed, int min, int max, ImGuiSliderFlags flags, std::string bgColor)
		{
			if (bgColor != "")
			{
				ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor(bgColor));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("drag"));
			}		
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Assets::assetManager.GetColor("dragHovered"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, Assets::assetManager.GetColor("dragActive"));

			if (width != 0)
			{
				ImGui::SetNextItemWidth(width);
			}
			else
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			}
			
			bool b_sliderChanged = ImGui::DragInt(ID.c_str(), &value, speed, min, max, "%d", flags | ImGuiSliderFlags_AlwaysClamp);
			
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
			}

			ImGui::PopStyleColor(3);

			return b_sliderChanged;
		}

		bool RenderDragLong(std::string ID, float width, long& value, std::string bgColor)
		{
			if (bgColor != "")
			{
				ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor(bgColor));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("drag"));
			}		
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Assets::assetManager.GetColor("dragHovered"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, Assets::assetManager.GetColor("dragActive"));

			if (width != 0)
			{
				ImGui::SetNextItemWidth(width);
			}
			else
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			}
			
			bool b_sliderChanged = ImGui::DragScalar(ID.c_str(), ImGuiDataType_S64, &value, 1.0f, NULL, NULL, NULL, ImGuiSliderFlags_AlwaysClamp);			
			
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
			}

			ImGui::PopStyleColor(3);

			return b_sliderChanged;
		}

		void PushSliderStyles()
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("sliderBg"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Assets::assetManager.GetColor("sliderHovered"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, Assets::assetManager.GetColor("sliderActive"));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, Assets::assetManager.GetColor("sliderGrab"));
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, Assets::assetManager.GetColor("sliderGrabActive"));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(2, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4);
			ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 10);
			ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 20);
		}

		void PopSliderStyles()
		{
			ImGui::PopStyleVar(4);
			ImGui::PopStyleColor(5);
		}

		// if label starts with ## it will not be included visually
		bool RenderSliderFloat(std::string label, float& value, float increment, float min, float max, float width, int digitsAfterDecimal)
		{
			std::string format = "%." + std::to_string(digitsAfterDecimal) + "f";
			if (width == -1)
			{
				width = ImGui::GetContentRegionAvail().x;
			}

			ImGui::SetNextItemWidth(width);
			PushSliderStyles();
			bool b_sliderChanged = ImGui::SliderFloat(label.c_str(), &value, min, max, format.c_str(), 0);
			PopSliderStyles();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			}

			return b_sliderChanged;
		}

		// if label starts with ## it will not be included visually
		bool RenderSliderInt(std::string label, int& value, int increment, int min, int max, float width)
		{
			if (width == -1)
			{
				width = ImGui::GetContentRegionAvail().x;
			}

			ImGui::SetNextItemWidth(width);
			PushSliderStyles();
			bool b_sliderChanged = ImGui::SliderInt(label.c_str(), &value, min, max, "%d", 0);
			PopSliderStyles();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			}

			return b_sliderChanged;
		}

		bool RenderCheckbox(std::string text, bool& b_toCheck)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("checkboxBg"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Assets::assetManager.GetColor("checkboxHovered"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, Assets::assetManager.GetColor("checkboxActive"));
			ImGui::PushStyleColor(ImGuiCol_CheckMark, Assets::assetManager.GetColor("checkboxCheck"));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(0));

			bool b_checked = ImGui::Checkbox(text.c_str(), &b_toCheck);

			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor(4);

			return b_checked;
		}

		void RenderSectionHeader(std::string headerText, float topPadding, float bottomPadding, std::string color, std::string separatorColor)
		{
			Vector2 headerP0 = ImGui::GetCursorScreenPos();
			headerP0.y += topPadding;
			GuiCore::RenderSeparator(topPadding, 3, separatorColor);
			auto winSize = ImGui::GetWindowSize();			
			ImGui::GetWindowDrawList()->AddRectFilled({ headerP0.x, headerP0.y - 3 }, { headerP0.x + winSize.x, headerP0.y + 17 }, Assets::assetManager.GetColor32(color), 0);
			ImGui::Text(" %s", headerText.c_str());
			GuiCore::RenderSeparator(4, bottomPadding, separatorColor);
		}

		// *** SECOND VECTOR IS THE SIZE, **NOT*** THE END POSITION. *** Sets CursorScreenPos to the starting point! *** 
		bool RenderInvisibleButton(std::string ID, Vector2 startingPoint, Vector2 size, bool b_allowOverlap, bool b_showRect, ImGuiButtonFlags flags)
		{
			if (size.x > 0 && size.y > 0)
			{
				if (b_showRect)
				{
					Logger::log.DrawRectangle(startingPoint, Vector2(startingPoint.x + size.x, startingPoint.y + size.y), "white", 1.0f, ImGui::GetWindowDrawList());
				}

				if (b_allowOverlap)
				{
					ImGui::SetNextItemAllowOverlap();
					flags += ImGuiButtonFlags_AllowOverlap; // 4096
				}

				ImGui::SetCursorScreenPos(startingPoint);
				return ImGui::InvisibleButton(ID.c_str(), size, flags);
			}
			else return false;
		}


		void BeginToolTip(std::string title)
		{		
			ImGui::BeginTooltip();
			ImGui::Text("%s", title.c_str());
			GuiCore::RenderSeparator(5, 5);
		}

		void EndToolTip()
		{		
			ImGui::EndTooltip();
		}

		void RenderToolTipText(std::string label, std::string text)
		{
			std::string newLabel = label + "  |  ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", text.c_str());
			GuiCore::RenderSeparator(5, 5);
		}

		void RenderToolTipFloat(std::string label, float data)
		{
			std::string newLabel = label + "  |  ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", std::to_string(data).c_str());
			GuiCore::RenderSeparator(5, 5);
		}

		void RenderToolTipLong(std::string label, long data)
		{
			std::string newLabel = label + "  |  ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", std::to_string(data).c_str());
			GuiCore::RenderSeparator(5, 5);
		}

		void RenderToolTipLongVector(std::string label, std::vector<long> data)
		{
			std::string newLabel = label + "  |  ";
			ImGui::Text("%s", newLabel.c_str());
			for (int i = 0; i < data.size(); i++)
			{
				std::string dataString = std::to_string(data[i]);
				if (i < data.size() - 1)
					dataString += ",";
				ImGui::SameLine();
				ImGui::Text("%s", dataString.c_str());
			}
			GuiCore::MoveScreenCursor(0, 5);
		}
		
		void RenderTextToolTip(std::string text)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(3, 3));
			ImGui::BeginTooltip();
			ImGui::Text("%s", text.c_str());
			ImGui::EndTooltip();
			ImGui::PopStyleVar();
		}
	}
}