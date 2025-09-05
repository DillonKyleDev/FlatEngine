#include "FlatEngine.h"
#include "FlatGui.h"
#include "TileSet.h"
#include "Vector2.h"

#include <string>
#include <vector>
#include "imgui.h"


namespace FL = FlatEngine;

namespace FlatGui
{
	void RenderTileSetEditor()
	{
		Vector2 centerPoint;
		static Vector2 scrolling = Vector2(0,0);
		static Vector2 gridStep = Vector2(50, 50);
		static std::string tileSetName = "";
		static bool b_openTileSetModal = false;
		static std::string tileSetTexturePath = "";
		int droppedValue = -1;
		TileSet* currentTileSet = nullptr;

		FL::BeginWindow("Tileset Editor", FG_b_showTileSetEditor);
		// {

			FL::BeginResizeWindowChild("Toolbar");
			// {

				if (FL::RenderButton("New TileSet"))
				{
					b_openTileSetModal = true;
				}

				FL::MoveScreenCursor(0, 5);
				ImGui::Text("Select TileSet to edit:");
				FL::MoveScreenCursor(0, 5);

				if (FL::F_TileSets.size() > 0)
				{
					currentTileSet = FL::GetTileSet(FL::F_selectedTileSetToEdit);

					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					FL::PushComboStyles();			
					if (ImGui::BeginCombo("##tileSets", FL::F_selectedTileSetToEdit.c_str()))
					{
						for (int i = 0; i < FL::F_TileSets.size(); i++)
						{
							bool b_selected = (FL::F_TileSets.at(i).GetName() == FL::F_selectedTileSetToEdit);
							ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("outerWindow"));
							if (ImGui::Selectable(FL::F_TileSets.at(i).GetName().c_str(), b_selected))
							{
								FL::F_selectedTileSetToEdit = FL::F_TileSets.at(i).GetName();
							}
							if (b_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
							ImGui::PopStyleColor();
						}
						ImGui::EndCombo();
					}
					FL::PopComboStyles();

					if (currentTileSet != nullptr)
					{
						tileSetTexturePath = currentTileSet->GetTexturePath();

						if (FL::DropInputCanOpenFiles("##TileSetSource", "File", FL::GetFilenameFromPath(tileSetTexturePath, true), FL::F_fileExplorerTarget, droppedValue, tileSetTexturePath, "Drop images here from the File Explorer"))
						{							
							if (droppedValue != -1 && FL::F_selectedFiles.size() >= droppedValue)
							{
								tileSetTexturePath = FL::F_selectedFiles[droppedValue - 1];
								currentTileSet->SetTexturePath(tileSetTexturePath);
								FL::SaveTileSet(*currentTileSet);
							}
						}

						ImGui::TextWrapped("Changes are automatically saved to the TileSet.");
					}
				}


				// New TileSet Modal Popup
				if (FL::RenderInputModal("Create New TileSet", "Enter a name for the TileSet", tileSetName, b_openTileSetModal))
				{
					FL::CreateNewTileSetFile(tileSetName, FL::GetDir("tileSets"));
				}

			// }
			FL::EndWindowChild(); // Toolbar



			// Vertical divider between grid space and toolbar area -----------------------------------------------------------------------
			ImGui::SameLine(0,0);



			centerPoint = Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
			Vector2 gridStart = ImGui::GetCursorScreenPos();
			Vector2 gridEnd = Vector2(gridStart.x + ImGui::GetContentRegionAvail().x, gridStart.y + ImGui::GetContentRegionAvail().y);
			FL::BeginWindowChild("Gridspace", FL::GetColor("outerWindow"), 16 | 8); // No Scrolling flags 
			// {

				Vector2 canvas_sz = ImGui::GetContentRegionAvail();
				Vector2 currentPos = ImGui::GetCursorScreenPos();
		
				ImGui::SetCursorScreenPos(currentPos);
				ImGui::SetNextItemAllowOverlap();
				if (canvas_sz.x > 0 && canvas_sz.y > 0)
				{
					AddSceneViewMouseControls("#TileSetEditorGrid", gridStart, canvas_sz, scrolling, centerPoint, gridStep);
				}

				FlatGui::RenderGridView(centerPoint, scrolling, false, gridStart, gridEnd, canvas_sz, gridStep, Vector2(0, 0), false);

				if (currentTileSet != nullptr)
				{
					std::shared_ptr<Texture> texture = currentTileSet->GetTexture();
					Vector2 tileSize = Vector2((float)currentTileSet->GetTileWidth(), (float)currentTileSet->GetTileHeight());
					VkDescriptorSet tileTexture = texture->GetTexture();
					float textureWidth = (float)texture->GetWidth();
					float textureHeight = (float)texture->GetHeight();
					Vector2 pivotOffset = Vector2(textureWidth / 2, 0);
					Vector2 spriteOffset = Vector2(tileSize.x / 2, tileSize.y / 2);
					bool b_spriteScalesWithZoom = true;
					int renderOrder = 1;
					Vector4 tintColor = FL::GetColor("white");
			
					FL::AddImageToDrawList(tileTexture, Vector2(0,0), centerPoint, textureWidth, textureHeight, Vector2(0, 0), Vector2(1, 1), b_spriteScalesWithZoom, gridStep.x, ImGui::GetWindowDrawList());
					for (std::pair<int, std::pair<Vector2, Vector2>> tile : currentTileSet->GetTileSet())
					{
						RenderTileSetEditorTile(tile, tileSize, scrolling, gridStart, canvas_sz, gridStep, currentTileSet);
					}
				}

			// }
			FL::EndWindowChild(); // Gridspace

		// }
		FL::EndWindow(); // Tileset Editor
	}


	void RenderTileSetEditorTile(std::pair<int, std::pair<Vector2, Vector2>> tile, Vector2 tileSize, Vector2 &scrolling, Vector2 gridStart, Vector2 canvas_sz, Vector2 &step, TileSet* tileSet)
	{
		int index = tile.first;
		bool b_selected = false;

		std::vector<int> selectedIndices = tileSet->GetTileSetIndices();
		for (int i = 0; i < selectedIndices.size(); i++)
		{
			if (index == selectedIndices[i])
			{
				b_selected = true;
			}
		}

		Vector2 uvStart = tile.second.first;
		Vector2 uvEnd = tile.second.second;
		float tileWidthInPx = step.x * (tileSize.x / FL::F_pixelsPerGridSpace);
		float hIndex = uvStart.x / tileSize.x;
		float tileHeightInPx = step.y * (tileSize.y / FL::F_pixelsPerGridSpace);
		float vIndex = uvStart.y / tileSize.y;

		float tileStartX = (hIndex * tileWidthInPx) + gridStart.x + scrolling.x;
		float tileStartY = (vIndex * tileHeightInPx) + gridStart.y + scrolling.y;
		Vector2 tileStart = Vector2(tileStartX, tileStartY);
		Vector2 tileEnd = Vector2(tileStartX + tileWidthInPx, tileStartY + tileHeightInPx);

		std::string invisibleButtonID = "##TileSetEditorTileToggle-" + std::to_string(index);
		ImGuiIO& inputOutput = ImGui::GetIO();
		Vector2 currentPos = ImGui::GetCursorScreenPos();		
		
		AddSceneViewMouseControls(invisibleButtonID, tileStart, Vector2(tileWidthInPx, tileHeightInPx), scrolling, gridStart, step, FL::GetColor32("tileSetTileBorder"));
		const bool b_isHovered = ImGui::IsItemHovered();
		const bool b_isActive = ImGui::IsItemActive(); 
		const bool b_isClicked = ImGui::IsItemClicked();

		if (b_isHovered)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTile"));
			ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTileBorder"));
		}
		else if (b_selected)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetSelectedTile"));
		}
		if (b_isClicked)
		{
			tileSet->ToggleTile(index);		
			FL::SaveTileSet(*tileSet);
		}
		if (b_isActive)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoldingTile"));
		}
	}
}