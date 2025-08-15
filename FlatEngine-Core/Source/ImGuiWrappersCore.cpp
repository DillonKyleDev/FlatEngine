#include "FlatEngine.h"
#include "TagList.h"
#include "Component.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Scene.h"
#include "Script.h"
#include "Button.h"
#include "Canvas.h"
#include "Animation.h"
#include "Audio.h"
#include "Text.h"
#include "CharacterController.h"
#include "Project.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace FL = FlatEngine;


namespace FlatEngine 
{
	// Flags
	ImGuiChildFlags F_autoResizeChildFlags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGuiChildFlags F_resizeChildFlags = ImGuiChildFlags_ResizeX | ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGuiChildFlags F_childFlags = ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGuiChildFlags F_headerFlags = ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize;
	//ImGuiTableFlags_Resizable
	ImGuiTableFlags F_tableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame;
	ImGuiTableFlags F_resizeableTableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
	ImGuiInputTextFlags F_inputFlags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

	float F_childPadding = 8;


	// ImGui Wrappers
	void MoveScreenCursor(float x, float y)
	{
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + x, ImGui::GetCursorScreenPos().y + y));
	}

	void RenderSeparator(float topPadding, float bottomPadding)
	{
		MoveScreenCursor(0, topPadding);
		ImGui::Separator();
		MoveScreenCursor(0, bottomPadding);
	}

	void RenderSubTitle(std::string title)
	{
		Vector2 titleStartPos = ImGui::GetCursorScreenPos();
		Vector2 titleEndPos = Vector2(titleStartPos.x + ImGui::GetContentRegionAvail().x, titleStartPos.y + 24);
		ImGui::GetWindowDrawList()->AddRectFilled(titleStartPos, titleEndPos, GetColor32("componentSubTitleBg"));
		MoveScreenCursor(5, 5);
		ImGui::Text(title.c_str());
		MoveScreenCursor(0, 12);
	}

	void BeginWindow(std::string name, Vector4 bgColor)
	{
		PushWindowStyles(bgColor);
		ImGui::Begin(name.c_str());		
		PopWindowStyles();

		PushScrollbarStyles();
		BeginWindowChild(name);
	}

	void BeginWindow(std::string name, bool& b_isOpen, Vector4 bgColor)
	{
		PushWindowStyles(bgColor);

		ImGui::Begin(name.c_str(), &b_isOpen);		
		PopWindowStyles();

		PushScrollbarStyles();
		BeginWindowChild(name);
	}

	void BeginWindow(std::string name, bool& b_isOpen, ImGuiWindowFlags flags, Vector4 bgColor)
	{
		PushWindowStyles(bgColor);
		ImGui::Begin(name.c_str(), &b_isOpen, flags);		
		PopWindowStyles();

		PushScrollbarStyles();
		BeginWindowChild(name);
	}

	void EndWindow()
	{
		EndWindowChild();
		PopScrollbarStyles();
		ImGui::End();
	}

	void BeginWindowChild(std::string title, Vector4 bgColor, ImGuiWindowFlags flags, Vector2 padding)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, bgColor);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding.x, padding.y));
		ImGui::BeginChild(title.c_str(), Vector2(0, 0), F_childFlags, flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();		
	}

	void BeginResizeWindowChild(std::string title, Vector4 bgColor, ImGuiWindowFlags flags, Vector2 padding)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, bgColor);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding.x, padding.y));
		ImGui::BeginChild(title.c_str(), Vector2(0, 0), F_resizeChildFlags, flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	void EndWindowChild()
	{
		ImGui::EndChild();
	}

	void PushWindowStyles(Vector4 bgColor)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1,1));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 3));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("frameBg"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("frameBgActive"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("frameBgHovered"));
		ImGui::PushStyleColor(ImGuiCol_Tab, GetColor("tab"));
		ImGui::PushStyleColor(ImGuiCol_TabActive, GetColor("tabActive"));
		ImGui::PushStyleColor(ImGuiCol_TabHovered, GetColor("tabHovered"));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocused, GetColor("tabUnfocused"));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, GetColor("tabUnfocusedActive"));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, GetColor("titleBg"));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, GetColor("titleBgActive"));
		ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, GetColor("titleBgCollapsed"));
		ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, GetColor("textSelectedBg"));
		ImGui::PushStyleColor(ImGuiCol_ResizeGrip, GetColor("resizeGrip"));
		ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, GetColor("resizeGripActive"));
		ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, GetColor("resizeGripHovered"));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, GetColor("popupBg"));
		ImGui::PushStyleColor(ImGuiCol_NavWindowingHighlight, GetColor("navWindowHighlight"));
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, GetColor("navHighlight"));
		ImGui::PushStyleColor(ImGuiCol_NavWindowingDimBg, GetColor("navWindowDimBg"));
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, GetColor("modalWindowDimBg"));
		ImGui::PushStyleColor(ImGuiCol_DockingPreview, GetColor("dockingPreview"));
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, GetColor("dockingPreviewEmpty"));
	}

	void PopWindowStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	void PushScrollbarStyles()
	{
		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, GetColor("scrollbarBg"));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, GetColor("scrollbarGrab"));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, GetColor("scrollbarGrabActive"));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, GetColor("scrollbarGrabHovered"));
	}

	void PopScrollbarStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void PushComboStyles()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 5));
		ImGui::PushStyleColor(ImGuiCol_Button, GetColor("comboArrow"));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, GetColor("comboArrowHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("comboBg"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("comboHovered"));
		// For Selectables
		ImGui::PushStyleColor(ImGuiCol_Header, GetColor("comboSelectable"));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, GetColor("comboSelected"));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, GetColor("comboHighlighted"));
	}

	void PopComboStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		// For Selectables
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	void PushMenuStyles()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 8));		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 5));
		ImGui::PushStyleColor(ImGuiCol_Header, GetColor("treeSelectableSelected"));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, GetColor("treeSelectableHovered"));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, GetColor("treeSelectableActive"));
	}

	void PopMenuStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	void PushTableStyles()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));		
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, GetColor("tableCellDark"));
		ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, GetColor("tableCellLight"));
	}

	void PopTableStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();		
		ImGui::PopStyleVar();
	}

	bool PushTable(std::string ID, int columns, ImGuiTableFlags flags, Vector2 outerSize)
	{
		PushTableStyles();

		bool b_beginTable = false;

		if (outerSize.x == 0 && outerSize.y == 0)
		{
			if (ImGui::GetContentRegionAvail().x != 0)
			{
				outerSize = Vector2(ImGui::GetContentRegionAvail().x - 1, 0);
				b_beginTable = ImGui::BeginTable(ID.c_str(), columns, flags, outerSize);
			}
			else
			{
				b_beginTable = ImGui::BeginTable(ID.c_str(), columns, flags);
			}
		}
		else if (outerSize.x == -1 && outerSize.y == -1)
		{
			b_beginTable = ImGui::BeginTable(ID.c_str(), columns, flags);
		}

		
		if (b_beginTable)
		{
			for (int i = 0; i < columns; i++)
			{
				std::string columnLabel = ID + std::to_string(i);
				ImGui::TableSetupColumn(columnLabel.c_str(), 0);
			}
		}
		else
		{
			PopTableStyles();
		}

		return b_beginTable;
	}

	bool RenderFloatDragTableRow(std::string ID, std::string fieldName, float& value, float increment, float min, float max)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		MoveScreenCursor(4, 4);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		ImGui::PopStyleVar();
		bool b_isChanged = RenderDragFloat(ID.c_str(), 0, value, increment, min, max);
		ImGui::PushID(ID.c_str());
		ImGui::PopID();

		return b_isChanged;
	}

	bool RenderIntSliderTableRow(std::string ID, std::string fieldName, int& value, int increment, int min, int max)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		MoveScreenCursor(4, 4);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		ImGui::PopStyleVar();
		PushSliderStyles();
		bool b_isChanged = RenderSliderInt(fieldName, value, increment, min, max);
		PopSliderStyles();
		ImGui::PushID(ID.c_str());
		ImGui::PopID();

		return b_isChanged;
	}

	bool RenderTagListTableRow(std::string ID, std::string fieldName, FL::TagList &tagList)
	{
		bool b_changed = false;
		bool b_hasTag = tagList.HasTag(fieldName);
		bool b_collidesTag = tagList.CollidesTag(fieldName);
		std::string hasTagID = "##" + fieldName + "CheckboxHasTagID";
		std::string collidesTagID = "##" + fieldName + "CheckboxCollideTagID";

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		MoveScreenCursor(0, 2);
		ImGui::Text(fieldName.c_str());

		ImGui::TableSetColumnIndex(1);
		MoveScreenCursor(0, 2);
		if (RenderCheckbox(hasTagID.c_str(), b_hasTag))
		{
			tagList.ToggleTag(fieldName);
			b_changed = true;
		}

		ImGui::TableSetColumnIndex(2);
		MoveScreenCursor(0, 2);
		if (RenderCheckbox(collidesTagID.c_str(), b_collidesTag))
		{
			tagList.ToggleCollides(fieldName);
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
		ImGui::Text(fieldName.c_str());
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
		ImGui::Text(fieldName.c_str());
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
		ImGui::Text(fieldName.c_str());
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
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		b_edited = RenderInput(ID, "", value, b_canOpenFiles);
		ImGui::PushID(ID.c_str());
		ImGui::PopID();

		return b_edited;
	}

	void RenderTextTableRow(std::string ID, std::string fieldName, std::string value1, std::string value2)
	{
		// Push uneditableTableTextColor text color
		ImGui::PushStyleColor(ImGuiCol_Text, GetColor("noEditTableText"));

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		// Set table cell bg color
		ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(GetColor("noEditTableRowFieldBg")));
		MoveScreenCursor(0, 2);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		// Set table cell bg color
		ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(GetColor("noEditTableRowValueBg")));
		ImGui::Text(value1.c_str());

		if (value2 != "")
		{
			ImGui::TableSetColumnIndex(2);
			// Set table cell bg color
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(GetColor("noEditTableRowValueBg")));
			ImGui::Text(value2.c_str());
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

	bool RenderInput(std::string ID, std::string label, std::string& value, bool b_canOpenFiles, float inputWidth, ImGuiInputTextFlags flags)
	{
		bool b_editedButton = false;
		bool b_editedInput = false;
		bool b_dragTargeted = false;
		char newPath[1024];
		strcpy_s(newPath, value.c_str());
		std::string pathString = label;
		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5,3));

		if (pathString != "")
		{
			pathString += ":";
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(pathString.c_str());
			ImGui::SameLine(0, 5);			
		}

		float textLength = label.size() * ImGui::GetFont()->FontSize;

		if (b_canOpenFiles && inputWidth == -1)
		{
			inputWidth = ImGui::GetContentRegionAvail().x - 31 - textLength;
		}
		else if (b_canOpenFiles)
		{
			inputWidth -= 31 - textLength;
		}
		else if (inputWidth == -1)
		{
			inputWidth = ImGui::GetContentRegionAvail().x;
		}
		else
		{
			inputWidth -= textLength;
		}


		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
		Vector2 inputStart = ImGui::GetCursorScreenPos();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("input"));
		ImGui::SetNextItemWidth(inputWidth);
		b_editedInput = ImGui::InputText(ID.c_str(), newPath, IM_ARRAYSIZE(newPath), flags);
		ImGui::PopStyleColor();
		Vector2 inputSize = Vector2(inputWidth, ImGui::GetCursorScreenPos().y - inputStart.y);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		if (b_canOpenFiles)
		{
			ImGui::SameLine(0, -1);

			std::string buttonId = ID + "openFileButton";
			if (RenderImageButton(buttonId.c_str(), GetTexture("openFile"), Vector2(16), 1, Vector2(3), GetColor("openFileButtonBg"), GetColor("imageButtonTint"), GetColor("openFileButtonHovered"), GetColor("imageButtonActive"), Vector2(0), Vector2(1)))
			{
				std::string assetPath = FL::OpenLoadFileExplorer();
				strcpy_s(newPath, assetPath.c_str());
				b_editedButton = true;
			}
		}

		ImGui::PopStyleVar();

		if (newPath != nullptr)
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
			ImGui::Text(label.c_str());
			ImGui::SameLine(0, 5);			
		}

		if (inputWidth == -1)
		{
			inputWidth = ImGui::GetContentRegionAvail().x - 1;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
		Vector2 inputStart = ImGui::GetCursorScreenPos();
		Vector2 inputSize = Vector2(inputWidth, ImGui::GetFontSize() * 1.65f);
		ImGui::GetWindowDrawList()->AddRectFilled(inputStart, Vector2(inputStart.x + inputSize.x, inputStart.y + inputSize.y), GetColor32("input"), 1);
		ImGui::SetCursorScreenPos(Vector2(inputStart.x + 3, inputStart.y + 1));
		ImGui::Text(displayValue.c_str());

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
		strcpy_s(newPath, openedFileValue.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
		ImGui::AlignTextToFramePadding();

		if (label != "")
		{
			label += ":";
			ImGui::Text(label.c_str());
			ImGui::SameLine(0, 5);			
		}

		if (inputWidth == -1)
		{
			inputWidth = ImGui::GetContentRegionAvail().x - 31;
		}
		else
		{
			inputWidth -= 31;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
		Vector2 inputStart = ImGui::GetCursorScreenPos();
		Vector2 inputSize = Vector2(inputWidth, ImGui::GetFontSize() * 1.65f);
		ImGui::GetWindowDrawList()->AddRectFilled(inputStart, Vector2(inputStart.x + inputSize.x, inputStart.y + inputSize.y), GetColor32("input"), 1);
		ImGui::SetCursorScreenPos(Vector2(inputStart.x + 3, inputStart.y + 1));
		ImGui::Text(displayValue.c_str());

		RenderInvisibleButton("##DropInputOpenFilesdropTarget", inputStart, inputSize, true, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
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
		if (ImGui::BeginPopupContextItem("##DropInputOpenFilesdropTarget"))
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

		ImGui::SameLine();

		std::string buttonId = ID + "openFileButton";		
		if (RenderImageButton(buttonId.c_str(), GetTexture("openFile"), Vector2(16), 1, Vector2(3), GetColor("openFileButtonBg"), GetColor("imageButtonTint"), GetColor("openFileButtonHovered"), GetColor("imageButtonActive"), Vector2(0), Vector2(1)))
		{
			std::string assetPath = FL::OpenLoadFileExplorer();
			strcpy_s(newPath, assetPath.c_str());
			b_editedButton = true;
		}

		if (newPath != nullptr)
		{
			openedFileValue = newPath;
		}

		ImGui::PopStyleVar();

		return b_editedButton || b_dragTargeted;
	}

	bool RenderCombo(std::string ID, std::string displayedValue, std::vector<std::string> options, int& currentOption, float width)
	{
		bool b_interactedWith = false;

		if (width != -1)
		{
			ImGui::SetNextItemWidth(width);
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

	bool RenderSelectable(std::string ID, std::vector<std::string> options, int& currentOption)
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
		PushMenuStyles();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		currentlySelected = " " + options[currentOption];

		if (options[currentOption] == "")
		{
			currentlySelected = empty;
			ImGui::PushStyleColor(ImGuiCol_Text, GetColor("logText"));
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
					ImGui::PushStyleColor(ImGuiCol_Text, GetColor("logText"));
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, GetColor("white"));
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
		PopMenuStyles();
		PopComboStyles();

		return b_selectionMade;
	}

	bool PushTreeList(std::string ID)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("innerWindow"));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
		PushMenuStyles();
		bool b_beginTable = ImGui::BeginTable(ID.c_str(), 1, F_tableFlags);

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

		ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, name.c_str());
		if (ImGui::IsItemClicked())
		{
			nodeClicked = name;
		}

		ImGui::PushID(treeID.c_str());
		ImGui::PopID();
	}

	bool RenderButton(std::string text, Vector2 size, float rounding, Vector4 color, Vector4 hoverColor, Vector4 activeColor)
	{
		bool b_isClicked;

		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 3));

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
	
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return b_isClicked;
	}

	bool RenderImageButton(std::string ID, SDL_Texture* texture, Vector2 size, float rounding, Vector2 padding, Vector4 bgColor, Vector4 tint, Vector4 hoverColor, Vector4 activeColor, Vector2 uvStart, Vector2 uvEnd)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);		
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
		

		bool b_isClicked = ImGui::ImageButton(ID.c_str(), texture, size, uvStart, uvEnd, GetColor("transparent"), tint);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();		
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return b_isClicked;
	}

	bool RenderDragFloat(std::string text, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags, std::string bgColor)
	{
		if (bgColor != "")
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor(bgColor));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("drag"));
		}
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("dragHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("dragActive"));

		if (width != 0)
		{
			ImGui::SetNextItemWidth(width);
		}
		else
		{
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		}

		bool b_sliderChanged = ImGui::DragFloat(text.c_str(), &value, increment, min, max, "%.3f", flags);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return b_sliderChanged;
	}

	bool RenderDragInt(std::string text, float width, int& value, float speed, int min, int max, ImGuiSliderFlags flags, std::string bgColor)
	{
		if (bgColor != "")
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor(bgColor));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("drag"));
		}		
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("dragHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("dragActive"));

		if (width != 0)
		{
			ImGui::SetNextItemWidth(width);
		}
		else
		{
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		}
		
		bool b_sliderChanged = ImGui::DragInt(text.c_str(), &value, speed, min, max, "%d", flags);
		
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();		

		return b_sliderChanged;
	}

	void PushSliderStyles()
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("sliderBg"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("sliderHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("sliderActive"));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, GetColor("sliderGrab"));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, GetColor("sliderGrabActive"));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 26);
	}

	void PopSliderStyles()
	{
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
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
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("checkboxBg"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("checkboxHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("checkboxActive"));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, GetColor("checkboxCheck"));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(0));

		bool b_checked = ImGui::Checkbox(text.c_str(), &b_toCheck);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return b_checked;
	}

	void RenderSectionHeader(std::string headerText, float height)
	{
		Vector2 cursorPos = ImGui::GetCursorScreenPos();
		Vector2 regionAvailable = ImGui::GetContentRegionAvail();

		ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, Vector2(cursorPos.x + regionAvailable.x, cursorPos.y + 30 + height), ImGui::GetColorU32(GetColor("sectionHeaderBg")));
		MoveScreenCursor(8, 8);
		ImGui::Text(headerText.c_str());		
		MoveScreenCursor(0, height + 8);
		ImGui::GetWindowDrawList()->AddRect(cursorPos, Vector2(cursorPos.x + regionAvailable.x, cursorPos.y + height + 30), ImGui::GetColorU32(GetColor("sectionHeaderBorder")));
	}

	// *** SECOND VECTOR IS THE SIZE, **NOT*** THE END POSITION. *** Sets CursorScreenPos to the starting point! *** 
	bool RenderInvisibleButton(std::string ID, Vector2 startingPoint, Vector2 size, bool b_allowOverlap, bool b_showRect, ImGuiButtonFlags flags)
	{
		if (b_showRect)
		{
			FL::DebugRectangle(startingPoint, Vector2(startingPoint.x + size.x, startingPoint.y + size.y), GetColor("white"), 1.0f, ImGui::GetWindowDrawList());
		}
;
		if (b_allowOverlap)
		{
			ImGui::SetNextItemAllowOverlap();
			flags += ImGuiButtonFlags_AllowOverlap; // 4096
		}

		ImGui::SetCursorScreenPos(startingPoint);
		return ImGui::InvisibleButton(ID.c_str(), size, flags);
	}

	void RenderTextToolTip(std::string text)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(3, 3));
		ImGui::BeginTooltip();
		ImGui::Text(text.c_str());
		ImGui::EndTooltip();
		ImGui::PopStyleVar();
	}

	void BeginToolTip(std::string title)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(6));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(6));
		ImGui::BeginTooltip();
		if (title != "")
		{
			ImGui::Text(title.c_str());
		}
	}

	void EndToolTip()
	{		
		ImGui::EndTooltip();		
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	void RenderToolTipText(std::string label, std::string text)
	{
		std::string newLabel = label + "| ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(text.c_str());		
	}

	void RenderToolTipFloat(std::string label, float data)
	{
		std::string newLabel = label + "| ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(data).c_str());		
	}

	void RenderToolTipLong(std::string label, long data)
	{
		std::string newLabel = label + "| ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(data).c_str());
	}

	void RenderToolTipLongVector(std::string label, std::vector<long> data)
	{
		std::string newLabel = label + "| ";
		ImGui::Text(newLabel.c_str());
		for (int i = 0; i < data.size(); i++)
		{
			std::string dataString = std::to_string(data[i]);
			if (i < data.size() - 1)
			{
				dataString += ",";
			}
			ImGui::SameLine();
			ImGui::Text(dataString.c_str());
		}				
	}

	// Returns true if a valid input was retrieved. ***Remember to use static std::strings for inputValue***
	bool RenderInputModal(std::string label, std::string description, std::string& inputValue, bool& b_openModal)
	{		
		if (b_openModal)
		{
			ImGui::OpenPopup(label.c_str());
			b_openModal = false;
		}

		bool b_validInput = false;

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));		
		ImGuiStyle& style = ImGui::GetStyle();
		Vector4 currentModalWindowDimBgColor = style.Colors[ImGuiCol_ModalWindowDimBg];
		//style.Colors[ImGuiCol_ModalWindowDimBg] = GetColor("white");
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(10));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(F_childPadding));
		if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{			
			ImGui::Text(description.c_str());
			MoveScreenCursor(0, 2);

			float inputWidth = ImGui::CalcTextSize(description.c_str()).x;
			if (inputWidth < 245)
			{
				inputWidth = 245;
			}
			
			std::string inputLabel = "##" + label;
			RenderInput(inputLabel.c_str(), "", inputValue, false, inputWidth);

			if (ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				b_validInput = inputValue != "";
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				b_validInput = false;
				ImGui::CloseCurrentPopup();
			}

			MoveScreenCursor(0, 5);

			if (RenderButton("OK", Vector2(50, 25)))
			{ 
				b_validInput = (inputValue != "");
				ImGui::CloseCurrentPopup(); 
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine(0, 8);
			if (RenderButton("Cancel", Vector2(50, 25)))
			{
				ImGui::CloseCurrentPopup();
			}			
			ImGui::EndPopup();
		}		
		//style.Colors[ImGuiCol_ModalWindowDimBg] = currentModalWindowDimBgColor;
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		return b_validInput;
	}

	bool RenderConfirmModal(std::string label, std::string description, bool& b_openModal)
	{
		bool b_confirm = false;

		if (b_openModal)
		{
			ImGui::OpenPopup(label.c_str());
			b_openModal = false;
		}

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(description.c_str());

			ImGui::Separator();

			float inputWidth = ImGui::CalcTextSize(description.c_str()).x;
			if (inputWidth < 250)
			{
				inputWidth = 250;
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				b_confirm = true;
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				b_confirm = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 145);
			if (ImGui::Button("Confirm", ImVec2(70, 0)))
			{
				b_confirm = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine(0, 5);
			if (ImGui::Button("Cancel", ImVec2(70, 0)))
			{
				b_confirm = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		return b_confirm;
	}
}