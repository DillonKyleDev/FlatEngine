#include "managers/Assets.h"
#include "GuiCore.h"
#include "GuiEditor.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

#include "imgui.h"
#include "tools/Vector4.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
	namespace GuiEditor
	{
		void RenderLabelTable(std::string ID, std::string label, float width, bool b_light, std::string bgColor)
		{
			FL::Vector4 color = FL::Assets::assetManager.GetColor(bgColor);
			if (!b_light)
				color *= 0.75f;

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, FL::Vector2(4,4));
			if (FL::GuiCore::PushTable(ID, 1, ImGuiTableFlags_RowBg, FL::Vector2(width, 0)))			
			{						 	
				ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor("noEditTableText"));
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);						
				FL::GuiCore::MoveScreenCursor(4, 0);		
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(color));
				ImGui::Text("%s", label.c_str());			
				ImGui::PushID(ID.c_str());
				ImGui::PopID();

				ImGui::PopStyleColor();				
				FL::GuiCore::PopTable();
			}
			ImGui::PopStyleVar();
		}

		void RenderVerticalSeparator(bool b_show)
		{
			if (b_show)
			{
				FL::Vector2 p0 = FL::Vector2(ImGui::GetCursorScreenPos().x - 1, ImGui::GetCursorScreenPos().y);			
				FL::Vector2 p1 = FL::Vector2(p0.x, p0.y + 21);
				ImGui::GetWindowDrawList()->AddLine(p0, p1, FL::Assets::assetManager.GetColor32("tableLabelVerticalSeparator"), 1.0f);
			}
		}

		void RenderStringTable(std::string ID, std::string label, std::string& value, FL::Vector2 tableSize, float labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (FL::GuiCore::PushTable("##" + ID + "Table", 1, FL::GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderInput(column1Label.c_str(), "", value);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				FL::GuiCore::PopTable();
			}
		}

		void RenderFloatTableColumns(std::string ID, std::string label, float& value, std::string labelColor, std::string valueColor, float labelWidth, int labelIndex, int valueIndex)
		{									
			float textWidth = ImGui::CalcTextSize(label.c_str()).x;
			float offsetX = (labelWidth - textWidth) * 0.5f;

			ImGui::TableSetColumnIndex(labelIndex);		
			if (labelColor != "")				
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(labelColor));	
			ImGui::AlignTextToFramePadding();	
			if (offsetX > 0.0f)		
				FL::GuiCore::MoveScreenCursor(offsetX, 0);
			ImGui::Text("%s", label.c_str());					
			ImGui::TableSetColumnIndex(valueIndex);	
			if (valueColor != "")						
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
			FL::GuiCore::RenderDragFloat(ID.c_str(), 0, value, 0.1f, -FLT_MAX, FLT_MAX);
		}

		void RenderVector2Table(std::string ID, std::string label, FL::Vector2& vec2, FL::Vector2 tableSize, float labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light, bool b_vertSeperator)
		{						
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
			std::string column2Label = ID + std::to_string(2);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::vector<float> widths { 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
						
			if (FL::GuiCore::PushTable("##" + ID + "Table", 4, FL::GuiCore::tableFlags, innerTableSize, widths))
			{
				ImGui::TableNextRow();			
				RenderFloatTableColumns(column1Label, "X", vec2.x, valueLabelColors[0], valueColor, 16, 0, 1);
				RenderFloatTableColumns(column2Label, "Y", vec2.y, valueLabelColors[1], valueColor, 16, 2, 3);
				FL::GuiCore::PopTable();
			}
		}

		void RenderVector3Table(std::string ID, std::string label, FL::Vector3& vec3, FL::Vector2 tableSize, float labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light, bool b_vertSeperator)
		{						
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
			std::string column2Label = ID + std::to_string(2);
			std::string column3Label = ID + std::to_string(3);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::vector<float> widths { 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			
			if (FL::GuiCore::PushTable("##" + ID + "Table", 6, FL::GuiCore::tableFlags, innerTableSize, widths))
			{
				ImGui::TableNextRow();					
				RenderFloatTableColumns(column1Label, "X", vec3.x, valueLabelColors[0], valueColor, 16, 0, 1);
				RenderFloatTableColumns(column2Label, "Y", vec3.y, valueLabelColors[1], valueColor, 16, 2, 3);
				RenderFloatTableColumns(column3Label, "Z", vec3.z, valueLabelColors[2], valueColor, 16, 4, 5);
				FL::GuiCore::PopTable();
			}
		}

		void RenderVector4Table(std::string ID, std::string label, FL::Vector4& vec4, FL::Vector2 tableSize, float labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light, bool b_vertSeperator)
		{						
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
			std::string column2Label = ID + std::to_string(2);
			std::string column3Label = ID + std::to_string(3);
			std::string column4Label = ID + std::to_string(4);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);
			
			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::vector<float> widths { 16, 0, 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";
			
			if (FL::GuiCore::PushTable("##" + ID + "Table", 8, FL::GuiCore::tableFlags, innerTableSize, widths))
			{
				ImGui::TableNextRow();			
				RenderFloatTableColumns(column1Label, "X", vec4.x, valueLabelColors[0], valueColor, 16, 0, 1);
				RenderFloatTableColumns(column2Label, "Y", vec4.y, valueLabelColors[1], valueColor, 16, 2, 3);
				RenderFloatTableColumns(column3Label, "Z", vec4.z, valueLabelColors[2], valueColor, 16, 4, 5);
				RenderFloatTableColumns(column4Label, "W", vec4.w, valueLabelColors[3], valueColor, 16, 6, 7);
				FL::GuiCore::PopTable();
			}
		}

		void RenderTransformTable(std::string ID, FL::Vector3& position, FL::Vector3& rotation, FL::Vector3& scale)
		{
			float labelWidth = 68;
			std::vector<std::string> valueColors = { "transformXBGLight", "transformYBGLight", "transformZBGLight", "transformWBGLight" };	
			FL::Vector2 tableSize = FL::Vector2(ImGui::GetContentRegionAvail().x, 0);

			RenderVector3Table("##TransformComponentTable", "POSITION", position, tableSize, labelWidth, "noEditTableRowFieldBg", valueColors); FL::GuiCore::MoveScreenCursor(0,-4);
			RenderVector3Table("##TransformComponentTable", "ROTATION", rotation, tableSize, labelWidth, "noEditTableRowFieldBg", valueColors, false); FL::GuiCore::MoveScreenCursor(0,-4);
			RenderVector3Table("##TransformComponentTable", "SCALE",    scale,    tableSize, labelWidth, "noEditTableRowFieldBg", valueColors);
		}

		void RenderFloatTable(std::string ID, std::string label, float& value, FL::Vector2 tableSize, float labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (FL::GuiCore::PushTable("##" + ID + "Table", 1, FL::GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column1Label.c_str(), 0, value, 0.1f, -FLT_MAX, FLT_MAX);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				FL::GuiCore::PopTable();
			}
		}

		void RenderDoubleTable(std::string ID, std::string label, double& value, FL::Vector2 tableSize, float labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (FL::GuiCore::PushTable("##" + ID + "Table", 1, FL::GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragDouble(column1Label.c_str(), 0, value, 0.1f);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				FL::GuiCore::PopTable();
			}
		}

		void RenderInt32Table(std::string ID, std::string label, int& value, FL::Vector2 tableSize, float labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (FL::GuiCore::PushTable("##" + ID + "Table", 1, FL::GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragInt(column1Label.c_str(), 0, value, 1, -INT_MAX, INT_MAX);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				FL::GuiCore::PopTable();
			}
		}

		void RenderInt64Table(std::string ID, std::string label, long& value, FL::Vector2 tableSize, float labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (FL::GuiCore::PushTable("##" + ID + "Table", 1, FL::GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragLong(column1Label.c_str(), 0, value);
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				FL::GuiCore::PopTable();
			}
		}

		void RenderBoolTable(std::string ID, std::string label, bool& value, FL::Vector2 tableSize, float labelWidth, std::string labelColor, bool b_light, bool b_vertSeperator)
		{
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
						
			if (labelWidth == 0) 
				labelWidth = ImGui::CalcTextSize(label.c_str()).x + 9;
			RenderLabelTable(column0Label, label, labelWidth, b_light, labelColor);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(b_vertSeperator);

			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			std::vector<std::string> trueFalse = { "false", "true" };
			int currentBool = value ? 1 : 0;
			if (FL::GuiCore::PushTable("##" + ID + "Table", 1, FL::GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				if (FL::GuiCore::RenderCombo(column1Label.c_str(), value ? "true" : "false", trueFalse, currentBool, innerTableSize.x)) 
					value = (bool)currentBool;			
				ImGui::PushID(ID.c_str());
				ImGui::PopID();
				FL::GuiCore::PopTable();
			}
		}

		void BeginToolTip(std::string title)
		{		
			ImGui::BeginTooltip();
			ImGui::Text("%s", title.c_str());
			FL::GuiCore::RenderSeparator(5, 5);
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
			FL::GuiCore::RenderSeparator(5, 5);
		}

		void RenderToolTipFloat(std::string label, float data)
		{
			std::string newLabel = label + "  |  ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", std::to_string(data).c_str());
			FL::GuiCore::RenderSeparator(5, 5);
		}

		void RenderToolTipLong(std::string label, long data)
		{
			std::string newLabel = label + "  |  ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", std::to_string(data).c_str());
			FL::GuiCore::RenderSeparator(5, 5);
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
			FL::GuiCore::MoveScreenCursor(0, 5);
		}
	}
}