#include "managers/Assets.h"
#include "GuiCore.h"
#include "GuiEditor.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"

#include "imgui.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
	namespace GuiEditor
	{
		void RenderTransformTable(std::string ID, FL::Vector3& position, FL::Vector3& rotation, FL::Vector3& scale)
		{
			float labelWidth = 68;
			if (FL::GuiCore::PushTable("##TransformProperties" + ID, 1, FL::GuiCore::tableFlags, FL::Vector2(labelWidth, 0)))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor("noEditTableText"));
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, FL::Vector2(4,4));

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);						
				FL::GuiCore::MoveScreenCursor(4, 0);		
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(FL::Assets::assetManager.GetColor("noEditTableRowValueBg")));
				ImGui::Text("POSITION");			
				ImGui::PushID("PositionFields");
				ImGui::PopID();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);						
				FL::GuiCore::MoveScreenCursor(4, 0);			
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(FL::Assets::assetManager.GetColor("noEditTableRowValueBg")));
				ImGui::Text("ROTATION");				
				ImGui::PushID("RotationFields");
				ImGui::PopID();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);						
				FL::GuiCore::MoveScreenCursor(4, 0);			
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(FL::Assets::assetManager.GetColor("noEditTableRowValueBg")));
				ImGui::Text("   SCALE");			
				ImGui::PushID("ScaleFields");
				ImGui::PopID();

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				FL::GuiCore::PopTable();
			}

			ImGui::SameLine(0,0);
			FL::Vector2 tableSize = FL::Vector2((ImGui::GetContentRegionAvail().x) / 3.0f, 0.0f);

			std::vector<float> widths { 16, 0 };
			FL::GuiCore::MoveScreenCursor(-1, 0);
			if (FL::GuiCore::PushTable("##TransformPropertiesX" + ID, 2, FL::GuiCore::tableFlags, tableSize, widths))
			{
				FL::GuiCore::RenderFloatDragTableRow("##XPosition" + ID, "X", position.x, 0.1f, -FLT_MAX, FLT_MAX, "transformXBGLight");
				FL::GuiCore::RenderFloatDragTableRow("##XRotation" + ID, "X", rotation.x, 0.5f, -FLT_MAX, FLT_MAX, "transformXBGDark");
				FL::GuiCore::RenderFloatDragTableRow("##XScale" + ID, "X", scale.x, 0.1f, 0.001f, 1000, "transformXBGLight");
				FL::GuiCore::PopTable();
			}
					
			ImGui::SameLine(0,0);
			FL::GuiCore::MoveScreenCursor(-1, 0);

			if (FL::GuiCore::PushTable("##TransformPropertiesY" + ID, 2, FL::GuiCore::tableFlags, tableSize, widths))
			{
				FL::GuiCore::RenderFloatDragTableRow("##YPosition" + ID, "Y", position.y, 0.1f, -FLT_MAX, FLT_MAX, "transformYBGLight");
				FL::GuiCore::RenderFloatDragTableRow("##YRotation" + ID, "Y", rotation.y, 0.5f, -FLT_MAX, FLT_MAX, "transformYBGDark");
				FL::GuiCore::RenderFloatDragTableRow("##YScaleDrag" + ID, "Y", scale.y, 0.1f, 0.001f, 1000, "transformYBGLight");
				FL::GuiCore::PopTable();
			}

			ImGui::SameLine(0,0);
			FL::GuiCore::MoveScreenCursor(-1, 0);		

			if (FL::GuiCore::PushTable("##TransformPropertiesZ" + ID, 2, FL::GuiCore::tableFlags, FL::Vector2(ImGui::GetContentRegionAvail().x, 0.0f), widths))
			{
				FL::GuiCore::RenderFloatDragTableRow("##ZPosition" + ID, "Z", position.z, 0.1f, -FLT_MAX, FLT_MAX, "transformZBGLight");
				FL::GuiCore::RenderFloatDragTableRow("##ZRotation" + ID, "Z", rotation.z, 0.5f, -FLT_MAX, FLT_MAX, "transformZBGDark");
				FL::GuiCore::RenderFloatDragTableRow("##ZScaleDrag" + ID, "Z", scale.z, 0.1f, 0.001f, 1000, "transformZBGLight");
				FL::GuiCore::PopTable();
			}		
		}

		void RenderVector2Table(std::string ID, std::string label, FL::Vector2& vec2, FL::Vector2 tableSize, float labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light)
		{						
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
			std::string column2Label = ID + std::to_string(2);
						
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, FL::Vector2(4,4));
			if (FL::GuiCore::PushTable(ID, 1, ImGuiTableFlags_RowBg, FL::Vector2(labelWidth, 0)))			
			{						 	
				ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor("noEditTableText"));
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);						
				FL::GuiCore::MoveScreenCursor(4, 0);		
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32("noEditTableRowValueBg"));
				ImGui::Text("%s", label.c_str());			
				ImGui::PushID(column0Label.c_str());
				ImGui::PopID();

				ImGui::PopStyleColor();				
				FL::GuiCore::PopTable();
			}
			ImGui::PopStyleVar();	
			ImGui::SameLine(0,0);
			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::vector<float> widths { 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "";			
			
			if (FL::GuiCore::PushTable("##" + column1Label + "Table", 4, FL::GuiCore::tableFlags, innerTableSize, widths))
			{
				ImGui::TableNextRow();			

				ImGui::TableSetColumnIndex(0);		
				if (labelColor != "")				
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueLabelColors[0]));	
				FL::GuiCore::MoveScreenCursor(4, 4);
				ImGui::Text("X");		
				ImGui::TableSetColumnIndex(1);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column1Label.c_str(), 0, vec2.x, 0.1f, -FLT_MAX, FLT_MAX);

				ImGui::TableSetColumnIndex(2);		
				if (labelColor != "")				
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueLabelColors[1]));	
				FL::GuiCore::MoveScreenCursor(5, 0);
				ImGui::Text("Y");		
				ImGui::TableSetColumnIndex(3);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column2Label.c_str(), 0, vec2.y, 0.1f, -FLT_MAX, FLT_MAX);

				FL::GuiCore::PopTable();
			}
		}

		void RenderVector3Table(std::string ID, std::string label, FL::Vector3& vec3, FL::Vector2 tableSize, float labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light)
		{						
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
			std::string column2Label = ID + std::to_string(2);
			std::string column3Label = ID + std::to_string(3);
						
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, FL::Vector2(4,4));
			if (FL::GuiCore::PushTable(ID, 1, ImGuiTableFlags_RowBg, FL::Vector2(labelWidth, 0)))			
			{						 	
				ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor("noEditTableText"));
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);						
				FL::GuiCore::MoveScreenCursor(4, 0);		
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32("noEditTableRowValueBg"));
				ImGui::Text("%s", label.c_str());			
				ImGui::PushID(column0Label.c_str());
				ImGui::PopID();

				ImGui::PopStyleColor();				
				FL::GuiCore::PopTable();
			}
			ImGui::PopStyleVar();	
			ImGui::SameLine(0,0);
			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::vector<float> widths { 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "";			
			
			if (FL::GuiCore::PushTable("##" + column1Label + "Table", 6, FL::GuiCore::tableFlags, innerTableSize, widths))
			{
				ImGui::TableNextRow();			

				ImGui::TableSetColumnIndex(0);		
				if (labelColor != "")				
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueLabelColors[0]));	
				FL::GuiCore::MoveScreenCursor(4, 4);
				ImGui::Text("X");		
				ImGui::TableSetColumnIndex(1);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column1Label.c_str(), 0, vec3.x, 0.1f, -FLT_MAX, FLT_MAX);

				ImGui::TableSetColumnIndex(2);		
				if (labelColor != "")				
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueLabelColors[1]));	
				FL::GuiCore::MoveScreenCursor(5, 0);
				ImGui::Text("Y");		
				ImGui::TableSetColumnIndex(3);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column2Label.c_str(), 0, vec3.y, 0.1f, -FLT_MAX, FLT_MAX);

				ImGui::TableSetColumnIndex(4);		
				if (labelColor != "")				
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueLabelColors[2]));	
				FL::GuiCore::MoveScreenCursor(4, 0);
				ImGui::Text("Z");		
				ImGui::TableSetColumnIndex(5);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column3Label.c_str(), 0, vec3.z, 0.1f, -FLT_MAX, FLT_MAX);

				FL::GuiCore::PopTable();
			}
		}

		void RenderVector4Table(std::string ID, std::string label, FL::Vector4& vec4, FL::Vector2 tableSize, float labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light)
		{						
			std::string column0Label = ID + std::to_string(0);
			std::string column1Label = ID + std::to_string(1);
			std::string column2Label = ID + std::to_string(2);
			std::string column3Label = ID + std::to_string(3);
			std::string column4Label = ID + std::to_string(4);
						
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, FL::Vector2(4,4));
			if (FL::GuiCore::PushTable(ID, 1, FL::GuiCore::tableFlags, FL::Vector2(labelWidth, 0)))			
			{						 	
				ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor("noEditTableText"));
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);						
				FL::GuiCore::MoveScreenCursor(4, 0);		
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32("noEditTableRowValueBg"));
				ImGui::Text("%s", label.c_str());			
				ImGui::PushID(column0Label.c_str());
				ImGui::PopID();

				ImGui::PopStyleColor();				
				FL::GuiCore::PopTable();
			}						
			ImGui::SameLine(0,0);
			FL::Vector2 innerTableSize = FL::Vector2((tableSize.x - labelWidth), 0.0f);
			std::vector<float> widths { 16, 0, 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "";
			ImGui::PopStyleVar();
			
			if (FL::GuiCore::PushTable("##" + column1Label + "Table", 8, FL::GuiCore::tableFlags, innerTableSize, widths))
			{
				ImGui::TableNextRow();			

				ImGui::TableSetColumnIndex(0);		
				if (labelColor != "")				
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueLabelColors[0]));	
				FL::GuiCore::MoveScreenCursor(4, 4);
				ImGui::Text("X");		
				ImGui::TableSetColumnIndex(1);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column1Label.c_str(), 0, vec4.x, 0.1f, -FLT_MAX, FLT_MAX);

				ImGui::TableSetColumnIndex(2);		
				if (labelColor != "")				
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueLabelColors[1]));	
				FL::GuiCore::MoveScreenCursor(5, 0);
				ImGui::Text("Y");		
				ImGui::TableSetColumnIndex(3);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column2Label.c_str(), 0, vec4.y, 0.1f, -FLT_MAX, FLT_MAX);

				ImGui::TableSetColumnIndex(4);		
				if (labelColor != "")				
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueLabelColors[2]));	
				FL::GuiCore::MoveScreenCursor(4, 0);
				ImGui::Text("Z");		
				ImGui::TableSetColumnIndex(5);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column3Label.c_str(), 0, vec4.z, 0.1f, -FLT_MAX, FLT_MAX);

				ImGui::TableSetColumnIndex(6);		
				if (labelColor != "")				
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueLabelColors[3]));	
				FL::GuiCore::MoveScreenCursor(4, 0);
				ImGui::Text("W");		
				ImGui::TableSetColumnIndex(7);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32(valueColor));	
				FL::GuiCore::RenderDragFloat(column4Label.c_str(), 0, vec4.w, 0.1f, -FLT_MAX, FLT_MAX);
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