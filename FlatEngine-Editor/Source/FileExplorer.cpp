#include "FlatGui.h"
#include "FlatEngine.h"
#include "Project.h"

#include "imgui.h"


namespace FL = FlatEngine;

namespace FlatGui
{
	std::map<std::string, bool> FG_fileExplorerLeafTracker = std::map<std::string, bool>();	
	std::map<std::string, std::shared_ptr<Texture>> FG_visibleThumbnails = std::map<std::string, std::shared_ptr<Texture>>();
	std::string FG_currentDirectory = "..\\";

	// "Local" values
	float topThumbnailPadding = 5;
	float horizontalThumbnailPadding = 4;
	Vector2 maxThumbnailSize = Vector2(100, 100);
	Vector2 iconButtonTextBoxSize = Vector2(110, 50);
	float thumbnailTextSpacing = 5;
	Vector2 iconButtonSize = Vector2(maxThumbnailSize.x + (horizontalThumbnailPadding * 2), maxThumbnailSize.y + thumbnailTextSpacing + iconButtonTextBoxSize.y + topThumbnailPadding);
	int maxCharactersPerFile = 10;
	int maxStoredLocations = 50;
	static std::vector<std::string> lastExplorerLocations = std::vector<std::string>();
	bool b_resetScroll = false;
	static bool b_openDeleteModal = false;
	static std::string fileToDelete = "";

	void RenderFileExplorer()
	{		
		std::string rootDirPath = FL::GetDir("projectDir");  // Relative to the solution
		std::filesystem::path rootPath(rootDirPath);
		std::error_code err;
		bool b_isDirectory = std::filesystem::is_directory(rootPath, err);

		FL::BeginWindow("File Explorer", FG_b_showFileExplorer);
		// {

			// Border around object
			auto directoriesWindowPos = ImGui::GetWindowPos();
			auto directoriesWindowSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
			//ImGui::GetWindowDrawList()->AddRect({ directoriesWindowPos.x + 2, directoriesWindowPos.y + 2 }, { directoriesWindowPos.x + directoriesWindowSize.x - 2, directoriesWindowPos.y + directoriesWindowSize.y - 2 }, FL::GetColor32("componentBorder"), 0);

			FL::BeginResizeWindowChild("Directories Panel", FL::GetColor("transparent"), 0, Vector2(0,0));
			// {
		
				FL::PushMenuStyles();
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0));
				ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, FL::GetColor32("fileExplorerTableRowBgAlt"));
				ImGui::PushStyleColor(ImGuiCol_TableRowBg, FL::GetColor32("fileExplorerTableRowBg"));
				FL::MoveScreenCursor(0, -1);
				if (ImGui::BeginTable("##DirectoriesTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersH |ImGuiTableFlags_SizingStretchSame))
				{
					ImGui::TableSetupColumn("##Directory", 0, ImGui::GetContentRegionAvail().x);

					if (b_isDirectory)
					{
						RenderDirNodes(rootDirPath);
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
				FL::PopMenuStyles();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();

			// }
			FL::EndWindowChild();




			// Vertical Division between hierarchy and view panel ------------------------------------------------
			ImGui::SameLine(0, 5);




			// New File Modals								
			static std::string newFileName = "";
			static bool b_openLuaModal = false;
			static bool b_openSceneModal = false;
			static bool b_openAnimationModal = false;
			static bool b_openMappingContextModal = false;
			static bool b_openTileSetModal = false;

			if (FL::RenderInputModal("Create Lua Script", "Enter a name for the Lua script", newFileName, b_openLuaModal))
			{
				FL::CreateNewLuaScript(newFileName, FG_currentDirectory);
			}
			if (FL::RenderInputModal("Create Scene", "Enter a name for the Scene", newFileName, b_openSceneModal))
			{
				FL::CreateNewSceneFile(newFileName, FG_currentDirectory);
			}
			if (FL::RenderInputModal("Create Animation", "Enter a name for the Animation", newFileName, b_openAnimationModal))
			{
				FL::CreateNewAnimationFile(newFileName, FG_currentDirectory);
			}
			if (FL::RenderInputModal("Create Mapping Context", "Enter a name for the Mapping Context", newFileName, b_openMappingContextModal))
			{
				FL::CreateNewMappingContextFile(newFileName, FG_currentDirectory);
			}
			if (FL::RenderInputModal("Create TileSet", "Enter a name for the TileSet", newFileName, b_openTileSetModal))
			{
				FL::CreateNewTileSetFile(newFileName, FG_currentDirectory);
			}


			FL::BeginWindowChild("Files Panel", FL::GetColor("transparent"), 0, Vector2(0,0));
			// {		
				
				FL::RenderSectionHeader("path:  " + FG_currentDirectory);

				RenderFilesTopBar();

				// File icons + border
				Vector2 borderStart = ImGui::GetCursorScreenPos();
				FL::BeginWindowChild("Files Container", FL::GetColor("explorerFilesPanelBg"));
				Vector2 iconsStart = ImGui::GetCursorScreenPos();
				Vector2 rightClickAreaSize = Vector2(ImGui::GetContentRegionMax().x, ImGui::GetContentRegionMax().y + ImGui::GetScrollY());
				Vector2 borderEnd = Vector2(ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x + 1, borderStart.y + rightClickAreaSize.y + 5);
				// {
				
					// Right clickable background button
					if (rightClickAreaSize.x > 0 && rightClickAreaSize.y > 0)
					{
						FL::RenderInvisibleButton("##FilesRightClickArea", borderStart, rightClickAreaSize, true, false, ImGuiButtonFlags_MouseButtonRight);

						// Right click menu
						if (ImGui::BeginPopupContextItem())
						{
							FL::PushMenuStyles();
							if (ImGui::BeginMenu("Add new..."))
							{
								if (ImGui::MenuItem("Lua Script"))
								{
									b_openLuaModal = true;														
									ImGui::CloseCurrentPopup();
								}
								if (ImGui::MenuItem("Scene"))
								{
									b_openSceneModal = true;
									ImGui::CloseCurrentPopup();
								}
								if (ImGui::MenuItem("Animation"))
								{
									b_openAnimationModal = true;
									ImGui::CloseCurrentPopup();
								}
								if (ImGui::MenuItem("Mapping Context"))
								{
									b_openMappingContextModal = true;
									ImGui::CloseCurrentPopup();
								}
								if (ImGui::MenuItem("TileSet"))
								{
									b_openTileSetModal = true;
									ImGui::CloseCurrentPopup();
								}
								ImGui::EndMenu();
							}
							FL::PopMenuStyles();

							ImGui::EndPopup();
						}
					}

					if (b_resetScroll)
					{
						ImGui::SetScrollY(0);
						b_resetScroll = false;
					}
					ImGui::SetCursorScreenPos(iconsStart - Vector2(0,6));
					RenderDirItems();


					if (FL::RenderConfirmModal("Are You Sure?", "Deleting this file cannot be undone", b_openDeleteModal))
					{
						FL::DeleteFileUsingPath(fileToDelete);
						b_openDeleteModal = false;
						fileToDelete = "";

						// Do specific things per file
						// If scene deleted was currently loaded, create new scene
						// If Animation was focused, unload it from the Animator
						// If the Mapping Context was selected, deselect it and reload Mapping Contexts
					}

				// }
				FL::EndWindowChild(); // Files Container

				ImGui::GetWindowDrawList()->AddRect(borderStart, borderEnd, FL::GetColor32("filesPanelOutline"));

			// }
			FL::EndWindowChild(); // Files Panel

		// }
		FL::EndWindow(); // File Explorer
	}

	void RenderDirNodes(std::string dir)
	{
		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			bool b_isDirectory = std::filesystem::is_directory(entry.path());

			RenderDirNode(entry.path());
		}
	}

	void RenderDirNode(std::filesystem::path fs_filepath)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGuiTreeNodeFlags nodeFlags;
		std::string treeID = fs_filepath.string() + "_node";				
		bool b_nodeOpen = false;		
		std::error_code err;
		bool b_isDirectory = std::filesystem::is_directory(fs_filepath, err);		
		
		// If this node is selected, use the node_selected to highlight it
		if (FG_currentDirectory == fs_filepath.string())
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
			if (FG_fileExplorerLeafTracker.count(treeID))
			{
				ImGui::SetNextItemOpen(FG_fileExplorerLeafTracker.at(treeID));
			}
			else
			{
				ImGui::SetNextItemOpen(false);
			}

			b_nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, fs_filepath.filename().string().c_str());
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				// save last location
				if (lastExplorerLocations.size() >= maxStoredLocations)
				{
					lastExplorerLocations.pop_back();
				}
				lastExplorerLocations.push_back(FG_currentDirectory);

				FG_currentDirectory = fs_filepath.string();
				SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				b_resetScroll = true; // Reset the scroll of the window
			}

			if (FG_fileExplorerLeafTracker.count(treeID))
			{
				FG_fileExplorerLeafTracker.at(treeID) = b_nodeOpen;
			}
			else
			{
				FG_fileExplorerLeafTracker.emplace(treeID, b_nodeOpen);
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
			ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor32("textLight"));
			ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, fs_filepath.filename().string().c_str());
			ImGui::PopStyleColor();
		}
	}

	void RenderFilesTopBar()
	{		
		ImGui::BeginDisabled(lastExplorerLocations.size() == 0);
		if (FL::RenderImageButton("##BackButtonFileExplorer", FL::GetTexture("back"), Vector2(20), 5, Vector2(1, 1), FL::GetColor("filesTopBarButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("filesTopBarButtonHover")))
		{
			// use last location
			FG_currentDirectory = lastExplorerLocations.back();
			SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
			b_resetScroll = true; // Reset the scroll of the window
			lastExplorerLocations.pop_back();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Back");
		
		ImGui::SameLine();

		std::filesystem::path parentDir(FG_currentDirectory);
		std::string parent = parentDir.stem().string();
		ImGui::BeginDisabled(parentDir.filename().string() == "..");
		if (FL::RenderImageButton("##UpButtonFileExplorer", FL::GetTexture("upDirectory"), Vector2(20), 5, Vector2(1), FL::GetColor("filesTopBarButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("filesTopBarButtonHover")))
		{
			// go up a directory
			lastExplorerLocations.push_back(FG_currentDirectory);
			FG_currentDirectory = parentDir.parent_path().string();
			SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
			b_resetScroll = true; // Reset the scroll of the window				
		}
		ImGui::EndDisabled();

		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Up directory");
		FL::MoveScreenCursor(0, 2);
	}

	void RenderDirItems()
	{
		float availableWidth = ImGui::GetContentRegionMax().x;
		float horizontalSpacing = 10;
		float verticalSpacing = 10;
		int maxIconsPerRow = (int)(availableWidth / (iconButtonSize.x + horizontalSpacing) - 1);
		int iconsThisRow = 0;		

		if (std::filesystem::is_directory(std::filesystem::path(FG_currentDirectory)))
		{
			for (const auto& entry : std::filesystem::directory_iterator(FG_currentDirectory))
			{				
				// Drawing the first button in the row
				if (iconsThisRow == 0)
				{
					ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + (horizontalSpacing / 2), (ImGui::GetCursorScreenPos().y + verticalSpacing)));
				}

				Vector2 currentPos = ImGui::GetCursorScreenPos();

				if (entry.path().extension() != ".prj" && entry.path().extension() != ".git")
				{
					RenderFileIcon(entry, currentPos);

					if (iconsThisRow != maxIconsPerRow)
					{
						ImGui::SetCursorScreenPos(Vector2(currentPos.x + iconButtonSize.x + horizontalSpacing, currentPos.y)); // Ready to draw the next button
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

	void RenderFileIcon(std::filesystem::path fs_filepath, Vector2 currentPos)
	{
		std::string extension = fs_filepath.extension().string();
		std::string icon;
		Vector2 filenamePadding = Vector2(5, 15);
		std::shared_ptr<Texture> thumbnail;
		SDL_Texture* texture = nullptr;		
		Vector2 dimensions;
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
			if (FG_visibleThumbnails.count(fs_filepath.string()) < 1)
			{
				// Create and save a Texture for the file if it is an image file so we can display as a thumbnail
				thumbnail = std::make_shared<Texture>(fs_filepath.string());
				FG_visibleThumbnails.emplace(fs_filepath.string(), thumbnail);
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
		else
		{
			icon = "unmarkedFile";
		}
	
		// If it's an image we have a saved Texture for, use it
		if  (FG_visibleThumbnails.count(fs_filepath.string()))
		{
			thumbnail = FG_visibleThumbnails.at(fs_filepath.string());
		}
		else  // Else use default PNG file icon
		{
			thumbnail = FL::GetTextureObject(icon);
		}

		texture = thumbnail->GetTexture();
		dimensions = Vector2((float)thumbnail->GetWidth(), (float)thumbnail->GetHeight());
		float xAspect = dimensions.x / dimensions.y;
		float yAspect = dimensions.y / dimensions.x;

		// Resizing thumbnails
		if (dimensions.x > maxThumbnailSize.x && dimensions.y < maxThumbnailSize.y)
		{
			dimensions = Vector2(maxThumbnailSize.x, maxThumbnailSize.x * yAspect);
		}
		else if (dimensions.y > maxThumbnailSize.y && dimensions.x < maxThumbnailSize.x)
		{
			dimensions = Vector2(maxThumbnailSize.y * xAspect, maxThumbnailSize.y);
		}
		else if (dimensions.x > maxThumbnailSize.x && dimensions.y > maxThumbnailSize.y)
		{
			if (dimensions.x - maxThumbnailSize.x > dimensions.y - maxThumbnailSize.y)
			{
				dimensions = Vector2(maxThumbnailSize.x, maxThumbnailSize.x * yAspect);
			}
			else
			{
				dimensions = Vector2(maxThumbnailSize.y * xAspect, maxThumbnailSize.y);
			}
		}
		
		bool b_buttonSelected = false;
		for (std::string clickedFile : FL::F_selectedFiles)
		{
			if (clickedFile == fs_filepath.string())
			{
				b_buttonSelected = true;
			}
		}

		std::string buttonID = "FileIcon-" + fs_filepath.string();
		int imageXOffset = (int)(((maxThumbnailSize.x - dimensions.x) / 2) + horizontalThumbnailPadding);
		int imageYOffset = (int)(((maxThumbnailSize.y - dimensions.y) / 2) + topThumbnailPadding);
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
			ImGui::GetWindowDrawList()->AddRectFilled(currentPos, Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::GetColor32("selectedFileBg"), 2);
			ImGui::GetWindowDrawList()->AddRect(currentPos, Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::GetColor32("selectedFileOutline"), 2);
		}
		else
		{
			ImGui::GetWindowDrawList()->AddRectFilled(currentPos, Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::GetColor32("normalFileBg"), 2);
			ImGui::GetWindowDrawList()->AddRect(currentPos, Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::GetColor32("normalFileOutline"), 2);
		}

		FL::RenderInvisibleButton(buttonID.c_str(), currentPos, iconButtonSize, true, false, ImGuiButtonFlags_MouseButtonLeft);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
			ImGui::GetWindowDrawList()->AddRectFilled(currentPos, Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::GetColor32("hoveredFileBg"), 2);
			ImGui::GetWindowDrawList()->AddRect(currentPos, Vector2(currentPos.x + iconButtonSize.x, currentPos.y + iconButtonSize.y), FL::GetColor32("hoveredFileOutline"), 2);
		}
		bool b_leftClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
		bool b_rightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
		bool b_doubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
		
		// Right click menu
		if (ImGui::BeginPopupContextItem())
		{
			FL::PushMenuStyles();
			if (ImGui::MenuItem(std::string("Open " + openIn).c_str()))
			{
				OpenFileContextually(fs_filepath);

				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Delete"))
			{
				b_openDeleteModal = true;
				fileToDelete = fs_filepath.string();
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Copy"))
			{

				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Paste"))
			{

				ImGui::CloseCurrentPopup();
			}
			FL::PopMenuStyles();

			ImGui::EndPopup();
		}

		// Multi select
		if (b_leftClicked || b_rightClicked)
		{
			if (!ImGui::GetIO().KeyCtrl)
			{
				FL::F_selectedFiles.clear();
			}

			bool b_alreadyClicked = false;

			for (std::vector<std::string>::iterator clickedFile = FL::F_selectedFiles.begin(); clickedFile != FL::F_selectedFiles.end(); clickedFile++)
			{
				if (*clickedFile == fs_filepath.string())
				{
					FL::F_selectedFiles.erase(clickedFile);
					b_alreadyClicked = true;
					break;
				}
			}

			if (!b_alreadyClicked)
			{
				FL::F_selectedFiles.push_back(fs_filepath.string());
			}
		}

		if (std::filesystem::is_directory(fs_filepath) && b_doubleClicked)
		{
			// save last location
			if (lastExplorerLocations.size() >= maxStoredLocations)
			{
				lastExplorerLocations.pop_back();
			}
			lastExplorerLocations.push_back(FG_currentDirectory);

			FG_currentDirectory = fs_filepath.string();
			SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
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
			int numSelectedFiles = (int)FL::F_selectedFiles.size();
			ImGui::SetDragDropPayload(FL::F_fileExplorerTarget.c_str(), &numSelectedFiles, sizeof(int));
			ImGui::Text("Use this file");
			ImGui::EndDragDropSource();
		}
		
		ImGui::SetCursorScreenPos(Vector2(currentPos.x + imageXOffset, currentPos.y + imageYOffset));
		ImGui::Image(texture, dimensions);
		
		ImGui::SetCursorScreenPos(Vector2(currentPos.x + filenameXOffset, currentPos.y + maxThumbnailSize.y + thumbnailTextSpacing + topThumbnailPadding));
		Vector2 startTextBoxPos = ImGui::GetCursorScreenPos();
		ImGui::TextWrapped(fileName.c_str());
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, startTextBoxPos.y + iconButtonTextBoxSize.y));
	}

	void OpenFileContextually(std::filesystem::path fs_filepath)
	{
		std::string extension = fs_filepath.extension().string();

		// Scene file
		if (extension == ".scn")
		{
			FL::LoadScene(fs_filepath.string());
			FL::F_LoadedProject.SetLoadedScenePath(fs_filepath.string());
			SaveCurrentProject();
		}
		// Mapping Context file
		else if (extension == ".mpc")
		{
			FL::F_selectedMappingContextName = FL::GetFilenameFromPath(fs_filepath.string());
			FG_b_showMappingContextEditor = true;
		}
		// Animation file
		else if (extension == ".anm")
		{
			SetFocusedAnimation(FL::LoadAnimationFile(fs_filepath.string()));			
			FG_b_showAnimator = true;	
		}
		// Lua file
		else if (extension == ".scp")
		{
			// Open script editor
			FL::LogError("Script Editor not yet implemented.");
		}
		// Project file
		else if (extension == ".prj")
		{
			LoadProject(fs_filepath.string());
		}
		// Project file
		else if (extension == ".tls")
		{
			FL::F_selectedTileSetToEdit = FL::GetFilenameFromPath(fs_filepath.string());
		}
	}
}