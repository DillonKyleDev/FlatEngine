#include "Animator.h"
#include "FileManager.h"
#include "GuiCore.h"
#include "managers/AnimationManager.h"
#include "managers/Assets.h"
#include "managers/Controls.h"
#include "managers/ProjectManager.h"
#include "managers/SceneManager.h"
#include "managers/Settings.h"
#include "Modals.h"
#include "render/VulkanManager.h"
#include "tools/FileHelper.h"

#include "imgui.h"
#include "tools/Vector2.h"

namespace FL = FlatEngine;


namespace FlatGui
{
	namespace FileManager
    {
		std::map<std::string, bool> fileExplorerLeafTracker = std::map<std::string, bool>();	
		std::map<std::string, std::shared_ptr<FL::Texture>> visibleThumbnails = std::map<std::string, std::shared_ptr<FL::Texture>>();		
		float topThumbnailPadding = 5;
		float horizontalThumbnailPadding = 4;		
		FL::Vector2 iconButtonTextBoxSize = FL::Vector2(110, 20);
		float thumbnailTextSpacing = 5;
		FL::Vector2 iconButtonSize = FL::Vector2(100.0f + (horizontalThumbnailPadding * 2), 100.0f + thumbnailTextSpacing + iconButtonTextBoxSize.y + topThumbnailPadding);
		int maxCharactersPerFile = 10;
		int maxStoredLocations = 50;
		static std::vector<std::string> lastExplorerLocations = std::vector<std::string>();
		bool b_resetScroll = false;
		static bool b_openDeleteModal = false;
		static std::string fileToDelete = "";


		void RenderDirNodes(std::string dir)
		{
			int IDCounter = 0;
			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				bool b_isDirectory = std::filesystem::is_directory(entry.path());

				RenderDirNode(entry.path(), IDCounter);
				IDCounter++;
			}
		}

		void RenderDirNode(std::filesystem::path fs_filepath, int IDCounter)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGuiTreeNodeFlags nodeFlags;
			std::string treeID = fs_filepath.string() + "_node_" + std::to_string(IDCounter);
			bool b_nodeOpen = false;		
			std::error_code err;
			bool b_isDirectory = std::filesystem::is_directory(fs_filepath, err);		
			
			// If this node is selected, use the node_selected to highlight it
			if (FL::ProjectManager::loadedProject.currentFileDirectory == fs_filepath.string())
			{
				if (b_isDirectory)
				{
					if (!std::filesystem::is_empty(fs_filepath))
					{
						nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
					}
					else
					{
						nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
					}
				}
				else
				{
					nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
				}
			}
			// Not selected
			else
			{
				if (b_isDirectory)
				{
					if (!std::filesystem::is_empty(fs_filepath))
					{
						nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
					}
					else
					{
						nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
					}
				}
				else
				{
					nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
				}
			}

			// if it is a directory
			if (b_isDirectory)
			{
				if (fileExplorerLeafTracker.count(treeID))
				{
					ImGui::SetNextItemOpen(fileExplorerLeafTracker.at(treeID));
				}
				else
				{
					ImGui::SetNextItemOpen(false);
				}

				b_nodeOpen = ImGui::TreeNodeEx(treeID.c_str(), nodeFlags, "%s", fs_filepath.filename().string().c_str());
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				{
					// save last location
					if (lastExplorerLocations.size() >= maxStoredLocations)
					{
						lastExplorerLocations.pop_back();
					}
					lastExplorerLocations.push_back(FL::ProjectManager::loadedProject.currentFileDirectory);

					FL::ProjectManager::loadedProject.currentFileDirectory = fs_filepath.string();		
					FL::ProjectManager::SaveCurrentProject();
					b_resetScroll = true; // Reset the scroll of the window
				}

				if (fileExplorerLeafTracker.count(treeID))
				{
					fileExplorerLeafTracker.at(treeID) = b_nodeOpen;
				}
				else
				{
					fileExplorerLeafTracker.emplace(treeID, b_nodeOpen);
				}

				if (b_nodeOpen)
				{
					RenderDirNodes(fs_filepath.string());
					ImGui::TreePop();
				}
			}
			// render a leaf
			else
			{			
				ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor32("textLight"));
				ImGui::TreeNodeEx(treeID.c_str(), nodeFlags, "%s", fs_filepath.filename().string().c_str());
				ImGui::PopStyleColor();
			}
		}

		void RenderFilesTopBar()
		{		
			FL::GuiCore::MoveScreenCursor(0, 2);
			ImGui::BeginDisabled(lastExplorerLocations.size() == 0);
			if (FL::GuiCore::RenderImageButton("##BackButtonFileExplorer", FL::Assets::assetManager.GetTexture("back"), FL::Vector2(16), 5, FL::Vector2(1), "buttonBorder", "filesTopBarButtonBg", "imageButtonTint", "filesTopBarButtonHover"))
			{
				// use last location
				FL::ProjectManager::loadedProject.currentFileDirectory = lastExplorerLocations.back();
				FL::ProjectManager::SaveCurrentProject();
				b_resetScroll = true; // Reset the scroll of the window
				lastExplorerLocations.pop_back();
			}
			ImGui::EndDisabled();

			ImGui::SameLine(0,3);

			std::filesystem::path parentDir(FL::ProjectManager::loadedProject.currentFileDirectory);
			std::string parent = parentDir.stem().string();
			ImGui::BeginDisabled(parentDir.filename().string() == "..");
			if (FL::GuiCore::RenderImageButton("##UpButtonFileExplorer", FL::Assets::assetManager.GetTexture("upDirectory"), FL::Vector2(16), 5, FL::Vector2(1), "buttonBorder", "filesTopBarButtonBg", "imageButtonTint", "filesTopBarButtonHover"))
			{
				// go up a directory
				lastExplorerLocations.push_back(FL::ProjectManager::loadedProject.currentFileDirectory);
				FL::ProjectManager::loadedProject.currentFileDirectory = parentDir.parent_path().string();
				FL::ProjectManager::SaveCurrentProject();
				b_resetScroll = true; // Reset the scroll of the window				
			}
			ImGui::EndDisabled();

			ImGui::SameLine();
			FL::GuiCore::MoveScreenCursor(0, 2);
			ImGui::Text("%s", FL::ProjectManager::loadedProject.currentFileDirectory.c_str());	
			
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 158, 0);

			ImGui::Image(FL::Assets::assetManager.GetTexture("search"), FL::Vector2(20)); ImGui::SameLine(0,0); FL::GuiCore::MoveScreenCursor(0, 2);
			ImGui::Image(FL::Assets::assetManager.GetTexture("minusLight"),  FL::Vector2(16)); ImGui::SameLine(0,2); FL::GuiCore::MoveScreenCursor(0,4);
			if (FL::GuiCore::RenderSliderFloat("##Zoom", FL::Settings::settings.fileExplorerThumbnailSize, 0.1f, 5, 100, 100, 2))
			{					
				FL::Settings::settings.SaveSettings();
			}
			ImGui::SameLine(0,2); FL::GuiCore::MoveScreenCursor(0, 2); ImGui::Image(FL::Assets::assetManager.GetTexture("plusLight"), FL::Vector2(16));

			iconButtonSize = FL::Vector2(FL::Settings::settings.fileExplorerThumbnailSize * 2 + (horizontalThumbnailPadding * 2), FL::Settings::settings.fileExplorerThumbnailSize * 2 + thumbnailTextSpacing + iconButtonTextBoxSize.y + topThumbnailPadding);				
		}

		void RenderFileIcon(std::filesystem::path fs_filepath, FL::Vector2 currentPos, int IDCounter)
		{
			std::string extension = fs_filepath.extension().string();
			std::string icon;
			FL::Vector2 filenamePadding = FL::Vector2(5, 15);
			std::shared_ptr<FL::Texture> thumbnail;
			VkDescriptorSet texture = nullptr;
			FL::Vector2 dimensions;
			std::string openIn = "";
			std::filesystem::path stem = fs_filepath.stem();
			
			if (std::filesystem::is_directory(fs_filepath))
			{
				icon = "folderFile";
			}
			else if (extension == ".cpp")
			{
				icon = "cppFile";
			}
			else if (extension == ".h")
			{
				icon = "hFile";
			}
			else if (extension == ".lua" && stem.extension() == ".scp")
			{
				icon = "luaFile";
				openIn = "in Script Editor";
			}
			else if (extension == ".png" || extension == ".jpg" || extension == ".tif" || extension == ".webp" || extension == ".jxl")
			{
				icon = "pngFile";
				if (visibleThumbnails.count(fs_filepath.string()) < 1)
				{
					// Create and save a Texture for the file if it is an image file so we can display as a thumbnail
					thumbnail = std::make_shared<FL::Texture>(fs_filepath.string());
					visibleThumbnails.emplace(fs_filepath.string(), thumbnail);
				}
			}
			else if (extension == ".mpc")
			{
				icon = "mapFile";
				openIn = "in Mapping Context Editor";
			}
			else if (extension == ".anm")
			{
				icon = "animFile";
				openIn = "in Animator";
			}
			else if (extension == ".wav" || extension == ".mp3")
			{
				icon = "audioFile";
			}
			else if (extension == ".scn")
			{
				icon = "sceneFile";
			}
			else if (extension == ".tls")
			{
				icon = "tileSetFile";
			}
			else if (extension == ".prf")
			{
				icon = "prefabFile";
			}
			else if (extension == ".mat")
			{
				icon = "materialFile";
				openIn = "in Material Editor";
			}
			else if (extension == ".obj")
			{
				icon = "objFile";
			}
			else
			{
				icon = "unmarkedFile";
			}
		
			// If it's an image we have a saved Texture for, use it
			if  (visibleThumbnails.count(fs_filepath.string()))
			{
				thumbnail = visibleThumbnails.at(fs_filepath.string());
			}
			else  // Else use default PNG file icon
			{
				thumbnail = FL::Assets::assetManager.GetTextureObject(icon);
			}

			texture = thumbnail->GetTexture();
			dimensions = FL::Vector2((float)thumbnail->GetWidth(), (float)thumbnail->GetHeight());
			float xAspect = dimensions.x / dimensions.y;
			float yAspect = dimensions.y / dimensions.x;

			if (dimensions.x < dimensions.y)
			{
				dimensions = FL::Vector2(FL::Settings::settings.fileExplorerThumbnailSize * 2 * xAspect, FL::Settings::settings.fileExplorerThumbnailSize * 2);
			}
			else if (dimensions.x > dimensions.y)
			{
				dimensions = FL::Vector2(FL::Settings::settings.fileExplorerThumbnailSize * 2, FL::Settings::settings.fileExplorerThumbnailSize * 2 * yAspect);
			}
			else 
			{
				dimensions = FL::Vector2(FL::Settings::settings.fileExplorerThumbnailSize * 2);
			}
			
			bool b_buttonSelected = false;
			for (std::string clickedFile : FL::GuiCore::selectedFiles)
			{
				if (clickedFile == fs_filepath.string())
				{
					b_buttonSelected = true;
				}
			}

			std::string buttonID = "FileIcon-" + fs_filepath.string() + "_" + std::to_string(IDCounter);
			int imageXOffset = (int)(((FL::Settings::settings.fileExplorerThumbnailSize * 2 - dimensions.x) / 2) + horizontalThumbnailPadding);
			int imageYOffset = (int)(((FL::Settings::settings.fileExplorerThumbnailSize * 2 - dimensions.y) / 2) + topThumbnailPadding);
			int textBoxXOffset = (int)(iconButtonSize.x - iconButtonTextBoxSize.x) / 2;

			std::string fileName = fs_filepath.filename().string();
			if (!b_buttonSelected && fs_filepath.filename().string().length() > maxCharactersPerFile)
			{
				fileName = fs_filepath.filename().string().substr(0, maxCharactersPerFile) + "..";
			}
			float textWidth = ImGui::CalcTextSize(fileName.c_str()).x;
			int filenameXOffset = (int)(iconButtonSize.x - textWidth) / 2;
			


			// Button interactions
			if (b_buttonSelected)
			{
				ImGui::GetWindowDrawList()->AddRectFilled(currentPos, FL::Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::Assets::assetManager.GetColor32("selectedFileBg"), 2);
				ImGui::GetWindowDrawList()->AddRect(currentPos, FL::Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::Assets::assetManager.GetColor32("selectedFileOutline"), 2);
			}
			else
			{
				ImGui::GetWindowDrawList()->AddRectFilled(currentPos, FL::Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::Assets::assetManager.GetColor32("normalFileBg"), 2);
				ImGui::GetWindowDrawList()->AddRect(currentPos, FL::Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::Assets::assetManager.GetColor32("normalFileOutline"), 2);
			}

			FL::GuiCore::RenderInvisibleButton(buttonID.c_str(), currentPos, iconButtonSize, true, false, ImGuiButtonFlags_MouseButtonLeft);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
				ImGui::GetWindowDrawList()->AddRectFilled(currentPos, FL::Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::Assets::assetManager.GetColor32("hoveredFileBg"), 2);
				ImGui::GetWindowDrawList()->AddRect(currentPos, FL::Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::Assets::assetManager.GetColor32("hoveredFileOutline"), 2);
			}
			bool b_leftClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
			bool b_rightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
			bool b_doubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
			
			// Right click menu
			FL::GuiCore::PushMenuStyles();
			if (ImGui::BeginPopupContextItem())
			{
				if (FL::GuiCore::MenuItem(std::string(" Open " + openIn).c_str()))
				{
					OpenFileContextually(fs_filepath);
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem(" Delete"))
				{
					b_openDeleteModal = true;
					fileToDelete = fs_filepath.string();
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem(" Copy"))
				{
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem(" Paste"))
				{
					ImGui::CloseCurrentPopup();
				}				
				ImGui::EndPopup();				
			}
			FL::GuiCore::PopMenuStyles();

			// Multi select
			if (b_leftClicked || b_rightClicked)
			{
				if (!ImGui::GetIO().KeyCtrl)
				{
					FL::GuiCore::selectedFiles.clear();
				}

				bool b_alreadyClicked = false;

				for (std::vector<std::string>::iterator clickedFile = FL::GuiCore::selectedFiles.begin(); clickedFile != FL::GuiCore::selectedFiles.end(); clickedFile++)
				{
					if (*clickedFile == fs_filepath.string())
					{
						FL::GuiCore::selectedFiles.erase(clickedFile);
						b_alreadyClicked = true;
						break;
					}
				}

				if (!b_alreadyClicked)
				{
					FL::GuiCore::selectedFiles.push_back(fs_filepath.string());
				}
			}

			if (std::filesystem::is_directory(fs_filepath) && b_doubleClicked)
			{
				// save last location
				if (lastExplorerLocations.size() >= maxStoredLocations)
				{
					lastExplorerLocations.pop_back();
				}
				lastExplorerLocations.push_back(FL::ProjectManager::loadedProject.currentFileDirectory);

				FL::ProjectManager::loadedProject.currentFileDirectory = fs_filepath.string();
				FL::ProjectManager::SaveCurrentProject();
				b_resetScroll = true; // Reset the scroll of the window
			}
			else if (b_doubleClicked)
			{
				OpenFileContextually(fs_filepath);
			}

			// Drag source
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
			{
				// Just saves the number of files selected and passes it to the drop source to query F_SelectedFiles
				int numSelectedFiles = (int)FL::GuiCore::selectedFiles.size() - 1;
				ImGui::SetDragDropPayload(FL::GuiCore::fileExplorerTarget.c_str(), &numSelectedFiles, sizeof(int));
				ImGui::Text("Use this file");
				ImGui::EndDragDropSource();
			}
			
			ImGui::SetCursorScreenPos(FL::Vector2(currentPos.x + imageXOffset, currentPos.y + imageYOffset));
			ImGui::Image(texture, dimensions);
			
			ImGui::SetCursorScreenPos(FL::Vector2(currentPos.x + filenameXOffset, currentPos.y + FL::Settings::settings.fileExplorerThumbnailSize * 2 + thumbnailTextSpacing + topThumbnailPadding));
			FL::Vector2 startTextBoxPos = ImGui::GetCursorScreenPos();
			ImGui::TextWrapped("%s", fileName.c_str());
			ImGui::SetCursorScreenPos(FL::Vector2(ImGui::GetCursorScreenPos().x, startTextBoxPos.y + iconButtonTextBoxSize.y));
		}
		
		void RenderDirItems()
		{
			float availableWidth = ImGui::GetContentRegionMax().x;
			float horizontalSpacing = 10;
			float verticalSpacing = 10;
			int maxIconsPerRow = (int)(availableWidth / (iconButtonSize.x + horizontalSpacing) - 1);
			int iconsThisRow = 0;		
			int IDCounter = 0;

			if (std::filesystem::is_directory(std::filesystem::path(FL::ProjectManager::loadedProject.currentFileDirectory)))
			{
				for (const auto& entry : std::filesystem::directory_iterator(FL::ProjectManager::loadedProject.currentFileDirectory))
				{				
					// Drawing the first button in the row
					if (iconsThisRow == 0)
					{
						ImGui::SetCursorScreenPos(FL::Vector2(ImGui::GetCursorScreenPos().x + (horizontalSpacing / 2), (ImGui::GetCursorScreenPos().y + verticalSpacing)));
					}

					FL::Vector2 currentPos = ImGui::GetCursorScreenPos();

					if (entry.path().extension() != ".prj" && entry.path().extension() != ".git")
					{
						RenderFileIcon(entry, currentPos, IDCounter);
						IDCounter++;

						if (iconsThisRow != maxIconsPerRow)
						{
							ImGui::SetCursorScreenPos(FL::Vector2(currentPos.x + iconButtonSize.x + horizontalSpacing, currentPos.y)); // Ready to draw the next button
							iconsThisRow++;
						}
						else
						{
							iconsThisRow = 0;
						}
					}
				}
			}
		}

		void OpenFileContextually(std::filesystem::path fs_filepath)
		{
			std::string extension = fs_filepath.extension().string();

			// Scene file
			if (extension == ".scn")
			{
				FL::SceneManager::LoadScene(fs_filepath.string());
				FL::ProjectManager::loadedProject.loadedScenePath = fs_filepath.string();
				FL::ProjectManager::SaveCurrentProject();
			}
			// Mapping Context file
			else if (extension == ".mpc")
			{
				FL::Controls::selectedMappingContextName = FL::FileHelper::GetFilenameFromPath(fs_filepath.string());
				FL::Settings::settings.b_showMappingContextEditor = true;
				FL::Settings::settings.SaveSettings();
			}
			// Animation file
			else if (extension == ".anm")
			{
				Animator::selectedKeyframe = nullptr;
				Animator::loadedAnimation = FL::AnimationManager::LoadAnimationFile(fs_filepath.string());	
				FL::ProjectManager::loadedProject.loadedAnimationPath = fs_filepath.string();			
				FL::ProjectManager::SaveCurrentProject();			
				FL::Settings::settings.b_showAnimator = true;	
				FL::Settings::settings.SaveSettings();
			}
			// Project file
			else if (extension == ".prj")
			{
				FL::ProjectManager::LoadProject(fs_filepath.string());
			}
			// TileSet file
			else if (extension == ".tls")
			{
				// FL::F_selectedTileSetToEdit = FL::GetFilenameFromPath(fs_filepath.string());
			}
			// Material file
			else if (extension == ".mat")
			{			
				FL::VulkanManager::selectedMaterialName = FL::FileHelper::GetFilenameFromPath(fs_filepath.string());
				FL::Settings::settings.b_showMaterialEditor = true;
				FL::Settings::settings.SaveSettings();
			}
		}
	}

	void RenderFileExplorer(bool& b_show)
	{		
		if (!b_show)
			return;
		
		std::string rootDirPath = FL::Assets::assetManager.GetDir("projectDir");  // Relative to the solution
		std::filesystem::path rootPath(rootDirPath);
		std::error_code err;
		bool b_isDirectory = std::filesystem::is_directory(rootPath, err);

		if (FL::GuiCore::BeginWindow("File Explorer", b_show))
		{
			// Border around object
			auto directoriesWindowPos = ImGui::GetWindowPos();
			auto directoriesWindowSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
			//ImGui::GetWindowDrawList()->AddRect({ directoriesWindowPos.x + 2, directoriesWindowPos.y + 2 }, { directoriesWindowPos.x + directoriesWindowSize.x - 2, directoriesWindowPos.y + directoriesWindowSize.y - 2 }, FL::Assets::assetManager.GetColor32("componentBorder"), 0);

			FL::GuiCore::BeginResizeWindowChild("Directories Panel", "transparent", 0, FL::Vector2());
			// {
		
				FL::GuiCore::PushTreeStyles();
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, FL::Vector2());
				ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, FL::Assets::assetManager.GetColor32("fileExplorerTableRowBgAlt"));
				ImGui::PushStyleColor(ImGuiCol_TableRowBg, FL::Assets::assetManager.GetColor32("fileExplorerTableRowBg"));
				FL::GuiCore::MoveScreenCursor(0, -1);
				if (ImGui::BeginTable("##DirectoriesTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersH |ImGuiTableFlags_SizingStretchSame))
				{
					ImGui::TableSetupColumn("##Directory", 0, ImGui::GetContentRegionAvail().x);

					if (b_isDirectory)
					{
						FileManager::RenderDirNodes(rootDirPath);
					}

					// Add empty table rows so the table goes all the way to the bottom of the screen
					float availableVerticalSpace = ImGui::GetContentRegionAvail().y + 9;
					while (availableVerticalSpace > 22)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
						ImGui::Text("");

						availableVerticalSpace = ImGui::GetContentRegionAvail().y + 9;
					}

					if (availableVerticalSpace > 1)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + availableVerticalSpace - 1);
					}

					ImGui::EndTable();
				}
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				FL::GuiCore::PopTreeStyles();

			// }
			FL::GuiCore::EndWindowChild();




			// Vertical Division between hierarchy and view panel ------------------------------------------------
			ImGui::SameLine(0, 5);




			FL::GuiCore::BeginWindowChild("Files Panel", "transparent", 0, FL::Vector2());
			// {								

				FileManager::RenderFilesTopBar();

				// File icons + border
				FL::Vector2 borderStart = ImGui::GetCursorScreenPos();
				FL::GuiCore::BeginWindowChild("Files Container", "explorerFilesPanelBg");
				FL::Vector2 iconsStart = ImGui::GetCursorScreenPos();
				FL::Vector2 rightClickAreaSize = FL::Vector2(ImGui::GetContentRegionMax().x, ImGui::GetContentRegionMax().y + ImGui::GetScrollY());
				FL::Vector2 borderEnd = FL::Vector2(ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x + 1, borderStart.y + rightClickAreaSize.y + 5);
				// {
				
					// Right clickable background button
					if (rightClickAreaSize.x > 0 && rightClickAreaSize.y > 0)
					{
						FL::GuiCore::RenderInvisibleButton("##FilesRightClickArea", borderStart, rightClickAreaSize, true, false, ImGuiButtonFlags_MouseButtonRight);

						// Right click menu
						FL::GuiCore::PushMenuStyles();
						if (ImGui::BeginPopupContextItem())
						{							
							if (ImGui::BeginMenu(" Add new..."))
							{
								if (FL::GuiCore::MenuItem(" Lua Script"))
								{
									Modals::b_openLuaModal = true;														
									ImGui::CloseCurrentPopup();
								}
								FL::GuiCore::RenderMenuSeparator();
								if (FL::GuiCore::MenuItem(" Scene"))
								{
									Modals::b_openSceneModal = true;
									ImGui::CloseCurrentPopup();
								}
								FL::GuiCore::RenderMenuSeparator();
								if (FL::GuiCore::MenuItem(" Animation"))
								{
									Modals::b_openAnimationModal = true;
									ImGui::CloseCurrentPopup();
								}
								FL::GuiCore::RenderMenuSeparator();
								if (FL::GuiCore::MenuItem(" Mapping Context"))
								{
									Modals::b_openMappingContextModal = true;
									ImGui::CloseCurrentPopup();
								}
								FL::GuiCore::RenderMenuSeparator();
								if (FL::GuiCore::MenuItem(" Material"))
								{
									Modals::b_openMaterialModal = true;
									ImGui::CloseCurrentPopup();
								}
								FL::GuiCore::RenderMenuSeparator();
								if (FL::GuiCore::MenuItem(" TileSet"))
								{
									Modals::b_openTileSetModal = true;
									ImGui::CloseCurrentPopup();
								}
								ImGui::EndMenu();
							}
							ImGui::EndPopup();							
						}
						FL::GuiCore::PopMenuStyles();
					}

					if (FileManager::b_resetScroll)
					{
						ImGui::SetScrollY(0);
						FileManager::b_resetScroll = false;
					}
					ImGui::SetCursorScreenPos(iconsStart - FL::Vector2(0,6));
					FileManager::RenderDirItems();

					if (Modals::RenderConfirmModal("Are You Sure?", "Deleting this file cannot be undone", FileManager::b_openDeleteModal))
					{
						FL::FileHelper::DeleteFileUsingPath(FileManager::fileToDelete);
						FileManager::b_openDeleteModal = false;
						FileManager::fileToDelete = "";

						// Do specific things per file
						// If scene deleted was currently loaded, create new scene
						// If Animation was focused, unload it from the Animator
						// If the Mapping Context was selected, deselect it and reload Mapping Contexts
					}

				// }
				FL::GuiCore::EndWindowChild(); // Files Container

				ImGui::GetWindowDrawList()->AddRect(borderStart, borderEnd, FL::Assets::assetManager.GetColor32("filesPanelOutline"));

			// }
			FL::GuiCore::EndWindowChild(); // Files Panel
		}	
		
		FL::GuiCore::EndWindow(); // File Explorer
	}
}