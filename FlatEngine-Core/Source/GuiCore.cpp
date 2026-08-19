#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/LuaManager.h"
#include "render/RenderWindow.h"
#include "render/VulkanManager.h"
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
		bool b_mouseDownCanWarp = false;
		Vector2 mouseDelta;
		Vector2 mousePos;
		Vector2 lastMousePos;

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
		bool b_currentTableLight = true;
		
		std::string fileExplorerTarget = "DND_FILE_PATH_OBJECT";
		std::string hierarchyTarget = "DND_HIERARCHY_OBJECT";

		std::vector<std::string> selectedFiles = std::vector<std::string>();
		CURSOR_MODE cursorMode = CURSOR_MODE::CURSOR_MODE_TRANSLATE;
		
		void PushTableStyles();
        void PopTableStyles();
        // bool PushTable(std::string ID, int columns, ImGuiTableFlags flags = tableFlags, Vector2 outerSize = Vector2(0), std::vector<float> widths = std::vector<float>());
        bool RenderFloatDragTableRow(std::string ID, std::string fieldName, float& value, float increment, float min, float max, std::string labelColor = "", std::string valueColor = "");
        bool RenderIntSliderTableRow(std::string ID, std::string fieldName, int& value, int increment, int min, int max, std::string color = "");        
        bool RenderIntDragTableRow(std::string ID, std::string fieldName, int& value, float speed, int min, int max);
        bool RenderCheckboxTableRow(std::string ID, std::string fieldName, bool& _value);
        void RenderSelectableTableRow(std::string ID, std::string fieldName, std::vector<std::string> options, int& currentOption);
        bool RenderInputTableRow(std::string ID, std::string fieldName, std::string& value, bool b_canOpenFiles = false);        
        // void PopTable();

		void SetupImGui()
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();			
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;			
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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
			// style.Colors[ImGuiCol_PopupBg]               = Assets::assetManager.GetColor("popupBg");
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
			style.Colors[ImGuiCol_MenuBarBg]            = Assets::assetManager.GetColor("menuBarBg");
			style.Colors[ImGuiCol_Border] 				= Assets::assetManager.GetColor("menuDropdownBorder");
			style.Colors[ImGuiCol_PopupBg] 				= Assets::assetManager.GetColor("menuDropdownBg");
			style.Colors[ImGuiCol_Header] 				= Assets::assetManager.GetColor("menuHeaderItem");
			style.Colors[ImGuiCol_HeaderActive] 		= Assets::assetManager.GetColor("menuHeaderItemActive");
			style.Colors[ImGuiCol_HeaderHovered] 		= Assets::assetManager.GetColor("menuHeaderItemHovered");
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
			style.ItemInnerSpacing = Vector2(4);
			style.ItemSpacing = Vector2(8,4);
			style.SeparatorTextAlign = Vector2(0.5f, 0.0f);
			style.SeparatorTextBorderSize = 1;
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
			b_mouseDownCanWarp = false;
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


		Vector2 GetMousePosWindow()
		{
			int globalX, globalY;
			SDL_GetGlobalMouseState(&globalX, &globalY);
			int windowX, windowY;
			SDL_GetWindowPosition(RenderWindow::window.GetWindow(), &windowX, &windowY);
			int windowWidth, windowHeight;
			SDL_GetWindowSize(RenderWindow::window.GetWindow(), &windowWidth, &windowHeight);

			return Vector2(globalX - windowX, globalY - windowY);		
		}

		void CalculateMouseDelta()
		{
			if (!GuiCore::b_mouseDownCanWarp)
			{	
				lastMousePos = GetMousePosWindow();
				mouseDelta = Vector2();
				return;
			}

			mousePos = GetMousePosWindow();
			mouseDelta = Vector2(mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y);	
			ImGui::GetIO().MouseDelta = ImVec2(mouseDelta.x, mouseDelta.y);

			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) || ImGui::IsMouseDragging(ImGuiMouseButton_Right))
			{
				int windowWidth, windowHeight;
				SDL_GetWindowSize(RenderWindow::window.GetWindow(), &windowWidth, &windowHeight);

				if (mousePos.x > windowWidth - 2)
				{
					SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), 1, mousePos.y);
					lastMousePos = Vector2(0, mousePos.y);
					mouseDelta.x = 1;
					ImGui::GetIO().MouseDelta = ImVec2(mouseDelta.x, mouseDelta.y);
					return;
				}
				else if (mousePos.x < 1)
				{
					SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), windowWidth - 2, mousePos.y);
					lastMousePos = Vector2(windowWidth - 1, mousePos.y);
					mouseDelta.x = -1;
					ImGui::GetIO().MouseDelta = ImVec2(mouseDelta.x, mouseDelta.y);
					return;
				}
				if (mousePos.y > windowHeight - 1)
				{
					SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), mousePos.x, 1);
					lastMousePos = Vector2(mousePos.x, 0);
					mouseDelta.y = 1;
					ImGui::GetIO().MouseDelta = ImVec2(mouseDelta.x, mouseDelta.y);
					return;
				}
				else if (mousePos.y < 1)
				{
					SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), mousePos.x, windowHeight - 1);
					lastMousePos = Vector2(mousePos.x, windowHeight);
					mouseDelta.y = 1;
					ImGui::GetIO().MouseDelta = ImVec2(mouseDelta.x, mouseDelta.y);
					return;
				}			
			}

			lastMousePos = mousePos;
		}
		// ImGui Wrappers
		void MoveScreenCursor(float x, float y)
		{
			if (x == 0 && y == 0)
				return;

			ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + x, ImGui::GetCursorScreenPos().y + y));
		}

		bool IsRootOfOpenMenuSet()
		{
			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = g.CurrentWindow;
			if ((g.OpenPopupStack.Size <= g.BeginPopupStack.Size) || (window->Flags & ImGuiWindowFlags_ChildMenu))
				return false;

			// Initially we used 'upper_popup->OpenParentId == window->IDStack.back()' to differentiate multiple menu sets from each others
			// (e.g. inside menu bar vs loose menu items) based on parent ID.
			// This would however prevent the use of e.g. PushID() user code submitting menus.
			// Previously this worked between popup and a first child menu because the first child menu always had the _ChildWindow flag,
			// making hovering on parent popup possible while first child menu was focused - but this was generally a bug with other side effects.
			// Instead we don't treat Popup specifically (in order to consistently support menu features in them), maybe the first child menu of a Popup
			// doesn't have the _ChildWindow flag, and we rely on this IsRootOfOpenMenuSet() check to allow hovering between root window/popup and first child menu.
			// In the end, lack of ID check made it so we could no longer differentiate between separate menu sets. To compensate for that, we at least check parent window nav layer.
			// This fixes the most common case of menu opening on hover when moving between window content and menu bar. Multiple different menu sets in same nav layer would still
			// open on hover, but that should be a lesser problem, because if such menus are close in proximity in window content then it won't feel weird and if they are far apart
			// it likely won't be a problem anyone runs into.
			const ImGuiPopupData* upper_popup = &g.OpenPopupStack[g.BeginPopupStack.Size];
			if (window->DC.NavLayerCurrent != upper_popup->ParentNavLayer)
				return false;
			return upper_popup->Window && (upper_popup->Window->Flags & ImGuiWindowFlags_ChildMenu) && ImGui::IsWindowChildOf(upper_popup->Window, window, true, false);
		}		
		bool BeginMenuBar()
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
				return false;
			if (!(window->Flags & ImGuiWindowFlags_MenuBar))
				return false;

			IM_ASSERT(!window->DC.MenuBarAppending);
			ImGui::BeginGroup(); // Backup position on layer 0 // FIXME: Misleading to use a group for that backup/restore
			ImGui::PushID("##MenuBar");

			// We don't clip with current window clipping rectangle as it is already set to the area below. However we clip with window full rect.
			// We remove 1 worth of rounding to Max.x to that text in long menus and small windows don't tend to display over the lower-right rounded area, which looks particularly glitchy.
			const float border_top = ImMax(IM_ROUND(window->WindowBorderSize * 0.5f - window->TitleBarHeight), 0.0f);
			const float border_half = IM_ROUND(window->WindowBorderSize * 0.5f);
			ImRect bar_rect = window->MenuBarRect();
			bar_rect.Max.y += ImGui::GetStyle().CellPadding.y * 2;
			ImRect clip_rect(ImFloor(bar_rect.Min.x + border_half), ImFloor(bar_rect.Min.y + border_top), ImFloor(ImMax(bar_rect.Min.x, bar_rect.Max.x - ImMax(window->WindowRounding, border_half))), ImFloor(bar_rect.Max.y));
			clip_rect.ClipWith(window->OuterRectClipped);
			ImGui::PushClipRect(clip_rect.Min, clip_rect.Max, false);

			// We overwrite CursorMaxPos because BeginGroup sets it to CursorPos (essentially the .EmitItem hack in EndMenuBar() would need something analogous here, maybe a BeginGroupEx() with flags).
			window->DC.CursorPos = window->DC.CursorMaxPos = ImVec2(bar_rect.Min.x + window->DC.MenuBarOffset.x, bar_rect.Min.y + window->DC.MenuBarOffset.y);
			window->DC.LayoutType = ImGuiLayoutType_Horizontal;
			window->DC.IsSameLine = false;
			window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
			window->DC.MenuBarAppending = true;
			ImGui::AlignTextToFramePadding();
			return true;
		}
		bool BeginMainMenuBar()
		{
			ImGuiContext& g = *GImGui;
			ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();

			// Notify of viewport change so GetFrameHeight() can be accurate in case of DPI change
			ImGui::SetCurrentViewport(NULL, viewport);
			
			ImVec2 cellPadding = ImGui::GetStyle().CellPadding;
			// For the main menu bar, which cannot be moved, we honor g.Style.DisplaySafeAreaPadding to ensure text can be visible on a TV set.
			// FIXME: This could be generalized as an opt-in way to clamp window->DC.CursorStartPos to avoid SafeArea?
			// FIXME: Consider removing support for safe area down the line... it's messy. Nowadays consoles have support for TV calibration in OS settings.
			g.NextWindowData.MenuBarOffsetMinVal = ImVec2(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
			float height = ImGui::GetFrameHeight() + cellPadding.y * 2;
			bool is_open = ImGui::BeginViewportSideBar("##MainMenuBar", viewport, ImGuiDir_Up, height, window_flags);
			g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);
			if (!is_open)
			{
				ImGui::End();
				return false;
			}

			// Temporarily disable _NoSavedSettings, in the off-chance that tables or child windows submitted within the menu-bar may want to use settings. (#8356)
			g.CurrentWindow->Flags &= ~ImGuiWindowFlags_NoSavedSettings;
			BeginMenuBar();
			return is_open;
		}
		void EndMainMenuBar()
		{
			ImGuiContext& g = *GImGui;
			if (!g.CurrentWindow->DC.MenuBarAppending)
			{
				IM_ASSERT_USER_ERROR(0, "Calling EndMainMenuBar() not from a menu-bar!"); // Not technically testing that it is the main menu bar
				return;
			}

			ImGui::EndMenuBar();
			g.CurrentWindow->Flags |= ImGuiWindowFlags_NoSavedSettings; // Restore _NoSavedSettings (#8356)

			// When the user has left the menu layer (typically: closed menus through activation of an item), we restore focus to the previous window
			// FIXME: With this strategy we won't be able to restore a NULL focus.
			if (g.CurrentWindow == g.NavWindow && g.NavLayer == ImGuiNavLayer_Main && !g.NavAnyRequest && g.ActiveId == 0)
				ImGui::FocusTopMostWindowUnderOne(g.NavWindow, NULL, NULL, ImGuiFocusRequestFlags_UnlessBelowModal | ImGuiFocusRequestFlags_RestoreFocusedChild);

			ImGui::End();
		}
		bool BeginMenuEx(const char* label, const char* icon, bool enabled)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
				return false;

			ImGuiContext& g = *GImGui;
			const ImGuiStyle& style = g.Style;
			const ImGuiID id = window->GetID(label);
			bool menu_is_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

			ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
			ImVec2 cellPadding = ImGui::GetStyle().CellPadding;
			label_size.x += cellPadding.x * 2;
			label_size.y += cellPadding.y * 2;

			// Sub-menus are ChildWindow so that mouse can be hovering across them (otherwise top-most popup menu would steal focus and not allow hovering on parent menu)
			// The first menu in a hierarchy isn't so hovering doesn't get across (otherwise e.g. resizing borders with ImGuiButtonFlags_FlattenChildren would react), but top-most BeginMenu() will bypass that limitation.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_ChildMenu | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;
			if (window->Flags & ImGuiWindowFlags_ChildMenu)
				window_flags |= ImGuiWindowFlags_ChildWindow;

			// If a menu with same the ID was already submitted, we will append to it, matching the behavior of Begin().
			// We are relying on a O(N) search - so O(N log N) over the frame - which seems like the most efficient for the expected small amount of BeginMenu() calls per frame.
			// If somehow this is ever becoming a problem we can switch to use e.g. ImGuiStorage mapping key to last frame used.
			if (g.MenusIdSubmittedThisFrame.contains(id))
			{
				if (menu_is_open)
					menu_is_open = ImGui::BeginPopupMenuEx(id, label, window_flags); // menu_is_open can be 'false' when the popup is completely clipped (e.g. zero size display)
				else
					g.NextWindowData.ClearFlags();          // we behave like Begin() and need to consume those values
				return menu_is_open;
			}

			// Tag menu as used. Next time BeginMenu() with same ID is called it will append to existing menu
			g.MenusIdSubmittedThisFrame.push_back(id);		

			// Odd hack to allow hovering across menus of a same menu-set (otherwise we wouldn't be able to hover parent without always being a Child window)
			// This is only done for items for the menu set and not the full parent window.
			const bool menuset_is_open = IsRootOfOpenMenuSet();
			if (menuset_is_open)
				ImGui::PushItemFlag(ImGuiItemFlags_NoWindowHoverableCheck, true);

			// The reference position stored in popup_pos will be used by Begin() to find a suitable position for the child menu,
			// However the final position is going to be different! It is chosen by FindBestWindowPosForPopup().
			// e.g. Menus tend to overlap each other horizontally to amplify relative Z-ordering.
			ImVec2 popup_pos, pos = window->DC.CursorPos;
			ImGui::PushID(label);
			if (!enabled)
				ImGui::BeginDisabled();
			const ImGuiMenuColumns* offsets = &window->DC.MenuColumns;
			bool pressed;

			// We use ImGuiSelectableFlags_NoSetKeyOwner to allow down on one menu item, move, up on another.
			const ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_NoHoldingActiveID | ImGuiSelectableFlags_NoSetKeyOwner | ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_NoAutoClosePopups;
			if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
			{
				// Menu inside a horizontal menu bar
				// Selectable extend their highlight by half ItemSpacing in each direction.
				// For ChildMenu, the popup position will be overwritten by the call to FindBestWindowPosForPopup() in Begin()
				popup_pos = ImVec2(pos.x - 1.0f - IM_TRUNC(style.ItemSpacing.x * 0.5f), pos.y + label_size.y - style.FramePadding.y + window->MenuBarHeight);
				window->DC.CursorPos.x += IM_TRUNC(style.ItemSpacing.x * 0.5f);
				// PushStyleVarX(ImGuiStyleVar_ItemSpacing, style.ItemSpacing.x * 2.0f);
				float w = label_size.x;
				ImVec2 text_pos(window->DC.CursorPos.x + cellPadding.x + offsets->OffsetLabel, window->DC.CursorPos.y + cellPadding.y + window->DC.CurrLineTextBaseOffset);
				pressed = ImGui::Selectable("", menu_is_open, selectable_flags, ImVec2(w, label_size.y));
				ImGui::LogSetNextTextDecoration("[", "]");
				ImGui::RenderText(text_pos, label);
				// PopStyleVar();
				window->DC.CursorPos.x += IM_TRUNC(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
			}
			else
			{
				// Menu inside a regular/vertical menu
				// (In a typical menu window where all items are BeginMenu() or MenuItem() calls, extra_w will always be 0.0f.
				//  Only when they are other items sticking out we're going to add spacing, yet only register minimum width into the layout system.
				popup_pos = ImVec2(pos.x, pos.y - style.WindowPadding.y);
				float icon_w = (icon && icon[0]) ? ImGui::CalcTextSize(icon, NULL).x : 0.0f;
				float checkmark_w = IM_TRUNC(g.FontSize * 1.20f);
				float min_w = window->DC.MenuColumns.DeclColumns(icon_w, label_size.x, 0.0f, checkmark_w); // Feedback to next frame
				float extra_w = ImMax(0.0f, ImGui::GetContentRegionAvail().x - min_w);
				ImVec2 text_pos(window->DC.CursorPos.x + cellPadding.x + offsets->OffsetLabel, window->DC.CursorPos.y + cellPadding.y + window->DC.CurrLineTextBaseOffset);
				pressed = ImGui::Selectable("", menu_is_open, selectable_flags | ImGuiSelectableFlags_SpanAvailWidth, ImVec2(min_w, label_size.y));
				ImGui::LogSetNextTextDecoration("", ">");
				ImGui::RenderText(text_pos, label);
				if (icon_w > 0.0f)
					ImGui::RenderText(ImVec2(pos.x + cellPadding.x + offsets->OffsetIcon, pos.y + cellPadding.y), icon);
				ImGui::RenderArrow(window->DrawList, ImVec2(pos.x + offsets->OffsetMark + extra_w + g.FontSize * 0.30f - 6.0f, pos.y + cellPadding.y), ImGui::GetColorU32(ImGuiCol_Text), ImGuiDir_Right);
			}
			if (!enabled)
				ImGui::EndDisabled();

			const bool hovered = (g.HoveredId == id) && enabled && !g.NavHighlightItemUnderNav;
			if (menuset_is_open)
				ImGui::PopItemFlag();

			bool want_open = false;
			bool want_open_nav_init = false;
			bool want_close = false;
			if (window->DC.LayoutType == ImGuiLayoutType_Vertical) // (window->Flags & (ImGuiWindowFlags_Popup|ImGuiWindowFlags_ChildMenu))
			{
				// Close menu when not hovering it anymore unless we are moving roughly in the direction of the menu
				// Implement http://bjk5.com/post/44698559168/breaking-down-amazons-mega-dropdown to avoid using timers, so menus feels more reactive.
				bool moving_toward_child_menu = false;
				ImGuiPopupData* child_popup = (g.BeginPopupStack.Size < g.OpenPopupStack.Size) ? &g.OpenPopupStack[g.BeginPopupStack.Size] : NULL; // Popup candidate (testing below)
				ImGuiWindow* child_menu_window = (child_popup && child_popup->Window && child_popup->Window->ParentWindow == window) ? child_popup->Window : NULL;
				if (g.HoveredWindow == window && child_menu_window != NULL)
				{
					const float ref_unit = g.FontSize; // FIXME-DPI
					const float child_dir = (window->Pos.x < child_menu_window->Pos.x) ? 1.0f : -1.0f;
					const ImRect next_window_rect = child_menu_window->Rect();
					ImVec2 ta = ImVec2(g.IO.MousePos.x - g.IO.MouseDelta.x, g.IO.MousePos.y - g.IO.MouseDelta.y);
					ImVec2 tb = (child_dir > 0.0f) ? next_window_rect.GetTL() : next_window_rect.GetTR();
					ImVec2 tc = (child_dir > 0.0f) ? next_window_rect.GetBL() : next_window_rect.GetBR();
					const float pad_farmost_h = ImClamp(ImFabs(ta.x - tb.x) * 0.30f, ref_unit * 0.5f, ref_unit * 2.5f); // Add a bit of extra slack.
					ta.x += child_dir * -0.5f;
					tb.x += child_dir * ref_unit;
					tc.x += child_dir * ref_unit;
					tb.y = ta.y + ImMax((tb.y - pad_farmost_h) - ta.y, -ref_unit * 8.0f); // Triangle has maximum height to limit the slope and the bias toward large sub-menus
					tc.y = ta.y + ImMin((tc.y + pad_farmost_h) - ta.y, +ref_unit * 8.0f);
					moving_toward_child_menu = ImTriangleContainsPoint(ta, tb, tc, g.IO.MousePos);
					//GetForegroundDrawList()->AddTriangleFilled(ta, tb, tc, moving_toward_child_menu ? IM_COL32(0,128,0,128) : IM_COL32(128,0,0,128)); // [DEBUG]
				}

				// The 'HovereWindow == window' check creates an inconsistency (e.g. moving away from menu slowly tends to hit same window, whereas moving away fast does not)
				// But we also need to not close the top-menu menu when moving over void. Perhaps we should extend the triangle check to a larger polygon.
				// (Remember to test this on BeginPopup("A")->BeginMenu("B") sequence which behaves slightly differently as B isn't a Child of A and hovering isn't shared.)
				if (menu_is_open && !hovered && g.HoveredWindow == window && !moving_toward_child_menu && !g.NavHighlightItemUnderNav && g.ActiveId == 0)
					want_close = true;

				// Open
				// (note: at this point 'hovered' actually includes the NavDisableMouseHover == false test)
				if (!menu_is_open && pressed) // Click/activate to open
					want_open = true;
				else if (!menu_is_open && hovered && !moving_toward_child_menu) // Hover to open
					want_open = true;
				else if (!menu_is_open && hovered && g.HoveredIdTimer >= 0.30f && g.MouseStationaryTimer >= 0.30f) // Hover to open (timer fallback)
					want_open = true;
				if (g.NavId == id && g.NavMoveDir == ImGuiDir_Right) // Nav-Right to open
				{
					want_open = want_open_nav_init = true;
					ImGui::NavMoveRequestCancel();
					ImGui::SetNavCursorVisibleAfterMove();
				}
			}
			else
			{
				// Menu bar
				if (menu_is_open && pressed && menuset_is_open) // Click an open menu again to close it
				{
					want_close = true;
					want_open = menu_is_open = false;
				}
				else if (pressed || (hovered && menuset_is_open && !menu_is_open)) // First click to open, then hover to open others
				{
					want_open = true;
				}
				else if (g.NavId == id && g.NavMoveDir == ImGuiDir_Down) // Nav-Down to open
				{
					want_open = true;
					ImGui::NavMoveRequestCancel();
				}
			}

			if (!enabled) // explicitly close if an open menu becomes disabled, facilitate users code a lot in pattern such as 'if (BeginMenu("options", has_object)) { ..use object.. }'
				want_close = true;
			if (want_close && ImGui::IsPopupOpen(id, ImGuiPopupFlags_None))
				ImGui::ClosePopupToLevel(g.BeginPopupStack.Size, true);

			IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Openable | (menu_is_open ? ImGuiItemStatusFlags_Opened : 0));
			ImGui::PopID();

			if (want_open && !menu_is_open && g.OpenPopupStack.Size > g.BeginPopupStack.Size)
			{
				// Don't reopen/recycle same menu level in the same frame if it is a different menu ID, first close the other menu and yield for a frame.
				ImGui::OpenPopup(label);
			}
			else if (want_open)
			{
				menu_is_open = true;
				ImGui::OpenPopup(label, ImGuiPopupFlags_NoReopen);// | (want_open_nav_init ? ImGuiPopupFlags_NoReopenAlwaysNavInit : 0));
			}

			if (menu_is_open)
			{
				ImGuiLastItemData last_item_in_parent = g.LastItemData;
				ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Always);                  // Note: misleading: the value will serve as reference for FindBestWindowPosForPopup(), not actual pos.
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, style.PopupRounding); // First level will use _PopupRounding, subsequent will use _ChildRounding
				menu_is_open = ImGui::BeginPopupMenuEx(id, label, window_flags); // menu_is_open may be 'false' when the popup is completely clipped (e.g. zero size display)
				ImGui::PopStyleVar();
				if (menu_is_open)
				{
					// Implement what ImGuiPopupFlags_NoReopenAlwaysNavInit would do:
					// Perform an init request in the case the popup was already open (via a previous mouse hover)
					if (want_open && want_open_nav_init && !g.NavInitRequest)
					{
						ImGui::FocusWindow(g.CurrentWindow, ImGuiFocusRequestFlags_UnlessBelowModal);
						ImGui::NavInitWindow(g.CurrentWindow, false);
					}

					// Restore LastItemData so IsItemXXXX functions can work after BeginMenu()/EndMenu()
					// (This fixes using IsItemClicked() and IsItemHovered(), but IsItemHovered() also relies on its support for ImGuiItemFlags_NoWindowHoverableCheck)
					g.LastItemData = last_item_in_parent;
					if (g.HoveredWindow == window)
						g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
				}
			}
			else
			{
				g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
			}

			return menu_is_open;
		}
		bool BeginMenu(const char* label, bool enabled)
		{
			return BeginMenuEx(label, NULL, enabled);
		}
		void EndMenu()
		{
			// Nav: When a left move request our menu failed, close ourselves.
			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = g.CurrentWindow;
			IM_ASSERT(window->Flags & ImGuiWindowFlags_Popup);  // Mismatched BeginMenu()/EndMenu() calls
			ImGuiWindow* parent_window = window->ParentWindow;  // Should always be != NULL is we passed assert.
			if (window->BeginCount == window->BeginCountPreviousFrame)
				if (g.NavMoveDir == ImGuiDir_Left && ImGui::NavMoveRequestButNoResultYet())
					if (g.NavWindow && (g.NavWindow->RootWindowForNav == window) && parent_window->DC.LayoutType == ImGuiLayoutType_Vertical)
					{
						ImGui::ClosePopupToLevel(g.BeginPopupStack.Size - 1, true);
						ImGui::NavMoveRequestCancel();
					}

			ImGui::EndPopup();
		}
		bool MenuItemEx(const char* label, const char* icon, const char* shortcut, bool selected, bool enabled)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
				return false;

			ImGuiContext& g = *GImGui;
			ImGuiStyle& style = g.Style;
			ImVec2 pos = window->DC.CursorPos;			
			ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
			ImVec2 cellPadding = ImGui::GetStyle().CellPadding;
			label_size.x += cellPadding.x * 2;
			label_size.y += cellPadding.y * 2;

			// See BeginMenuEx() for comments about this.
			const bool menuset_is_open = IsRootOfOpenMenuSet();
			if (menuset_is_open)
				ImGui::PushItemFlag(ImGuiItemFlags_NoWindowHoverableCheck, true);

			// We've been using the equivalent of ImGuiSelectableFlags_SetNavIdOnHover on all Selectable() since early Nav system days (commit 43ee5d73),
			// but I am unsure whether this should be kept at all. For now moved it to be an opt-in feature used by menus only.
			bool pressed;
			ImGui::PushID(label);
			if (!enabled)
				ImGui::BeginDisabled();

			// We use ImGuiSelectableFlags_NoSetKeyOwner to allow down on one menu item, move, up on another.
			const ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SelectOnRelease | ImGuiSelectableFlags_NoSetKeyOwner | ImGuiSelectableFlags_SetNavIdOnHover;
			const ImGuiMenuColumns* offsets = &window->DC.MenuColumns;
			if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
			{
				// Mimic the exact layout spacing of BeginMenu() to allow MenuItem() inside a menu bar, which is a little misleading but may be useful
				// Note that in this situation: we don't render the shortcut, we render a highlight instead of the selected tick mark.
				float w = label_size.x;
				window->DC.CursorPos.x += IM_TRUNC(style.ItemSpacing.x * 0.5f);
				ImVec2 text_pos(window->DC.CursorPos.x + offsets->OffsetLabel, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
				// PushStyleVarX(ImGuiStyleVar_ItemSpacing, style.ItemSpacing.x * 2.0f);
				pressed = ImGui::Selectable("", selected, selectable_flags, ImVec2(w, 0.0f));
				// PopStyleVar();
				if (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_Visible)
					ImGui::RenderText(text_pos, label);
				window->DC.CursorPos.x += IM_TRUNC(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
			}
			else
			{
				// Menu item inside a vertical menu
				// (In a typical menu window where all items are BeginMenu() or MenuItem() calls, extra_w will always be 0.0f.
				//  Only when they are other items sticking out we're going to add spacing, yet only register minimum width into the layout system.
				float icon_w = (icon && icon[0]) ? ImGui::CalcTextSize(icon, NULL).x : 0.0f;
				float shortcut_w = (shortcut && shortcut[0]) ? ImGui::CalcTextSize(shortcut, NULL).x : 0.0f;
				float checkmark_w = IM_TRUNC(g.FontSize * 1.20f);
				float min_w = window->DC.MenuColumns.DeclColumns(icon_w, label_size.x, shortcut_w, checkmark_w); // Feedback for next frame
				float stretch_w = ImMax(0.0f, ImGui::GetContentRegionAvail().x - min_w);
				pressed = ImGui::Selectable("", false, selectable_flags | ImGuiSelectableFlags_SpanAvailWidth, ImVec2(min_w, label_size.y));
				if (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_Visible)
				{
					ImGui::RenderText(ImVec2(pos.x + cellPadding.x + offsets->OffsetLabel, pos.y + cellPadding.y), label);
					if (icon_w > 0.0f)
						ImGui::RenderText(ImVec2(pos.x + cellPadding.x + offsets->OffsetIcon, pos.y + cellPadding.y), icon);
					if (shortcut_w > 0.0f)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
						ImGui::LogSetNextTextDecoration("(", ")");
						ImGui::RenderText(ImVec2(pos.x + cellPadding.x + offsets->OffsetShortcut + stretch_w, pos.y + cellPadding.y), shortcut, NULL, false);
						ImGui::PopStyleColor();
					}
					if (selected)
						ImGui::RenderCheckMark(window->DrawList, ImVec2(pos.x + offsets->OffsetMark + stretch_w + g.FontSize * 0.40f - 6.0f, pos.y + g.FontSize * 0.134f * 0.5f + cellPadding.y), ImGui::GetColorU32(ImGuiCol_Text), g.FontSize * 0.866f);
				}
			}
			IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (selected ? ImGuiItemStatusFlags_Checked : 0));
			if (!enabled)
				ImGui::EndDisabled();
			ImGui::PopID();
			if (menuset_is_open)
				ImGui::PopItemFlag();

			return pressed;
		}
		bool MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
		{
			return MenuItemEx(label, NULL, shortcut, selected, enabled);
		}
		bool MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled)
		{
			if (MenuItemEx(label, NULL, shortcut, p_selected ? *p_selected : false, enabled))
			{
				if (p_selected)
					*p_selected = !*p_selected;
				return true;
			}
			return false;
		}

		void RenderMenuSeparator(bool b_light)
		{
			std::string separatorColor = b_light ? "menuSeparatorLight" : "menuSeparator";
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			RenderSeparator(0,0, separatorColor);
			ImGui::PopStyleVar();
		}

		void RenderSeparator(float topPadding, float bottomPadding, std::string separatorColor)
		{
			MoveScreenCursor(0, topPadding);
			ImGui::PushStyleColor(ImGuiCol_Separator, Assets::assetManager.GetColor(separatorColor));
			Vector2 cursorPos = ImGui::GetCursorScreenPos();
			ImGui::Separator();
			cursorPos = ImGui::GetCursorScreenPos();
			ImGui::PopStyleColor();
			MoveScreenCursor(0, bottomPadding);
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

		void PushCellSpacingStyles()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(4, 4));				
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 8));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, Vector2(4, 8));		
		}
		void PopCellSpacingStyles()
		{
			ImGui::PopStyleVar(4);
		}

		void PushComboStyles()
		{
			PushCellSpacingStyles();
			// ImGui::PushStyleColor(ImGuiCol_Button, Assets::assetManager.GetColor("comboArrow"));
			// ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Assets::assetManager.GetColor("comboArrowHovered"));
			// ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("comboBg"));
			// ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Assets::assetManager.GetColor("comboHovered"));		
			// ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Assets::assetManager.GetColor("comboHovered"));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("comboBg"));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Assets::assetManager.GetColor("comboHovered"));			
		}
		void PopComboStyles()
		{
			ImGui::PopStyleColor(2);
			PopCellSpacingStyles();
		}

		void PushTreeStyles()
		{
			PushCellSpacingStyles();						
		}
		void PopTreeStyles()
		{
			PopCellSpacingStyles();		
		}
		
		void PushMenuStyles()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));	
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 0));		
			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, Vector2(0,0));	
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(0,0));	
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(6, 3));		
		}
		void PopMenuStyles()
		{
			ImGui::PopStyleVar(5);			
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

		bool RenderIntDragTableRow(std::string ID, std::string fieldName, int& value, float increment, int min, int max)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			MoveScreenCursor(4, 4);
			ImGui::Text("%s", fieldName.c_str());
			ImGui::TableSetColumnIndex(1);
			ImGui::PopStyleVar();
			ImGui::SetNextItemWidth(-1);
			bool b_isChanged = RenderDragInt(ID.c_str(), 0, value, increment, min, max);
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

		void PopTable()
		{
			ImGui::EndTable();
			PopTableStyles();
		}

		// Newer table functions
		void RenderLabelTable(TableProps tableProps)
		{
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;

			Vector4 color = Assets::assetManager.GetColor(tableProps.labelColor);
			if (!b_light)
				color *= 0.75f;

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(4,4));
			if (GuiCore::PushTable(tableProps.ID, 1, tableProps.flags, Vector2(tableProps.labelWidth, 0)))			
			{						 	
				ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("noEditTableText"));
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);						
				GuiCore::MoveScreenCursor(4, 0);		
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(color));
				ImGui::Text("%s", tableProps.label.c_str());			
				ImGui::PushID(tableProps.ID.c_str());
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

		bool RenderStringTable(TableProps tableProps, std::string& value)
		{
			bool b_changed = false;	
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
						
			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableProps.tableSize.x / 2 ? tableProps.tableSize.x / 2 : ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps.b_vertSeperator);

			Vector2 innerTableSize = Vector2((tableProps.tableSize.x - tableProps.labelWidth), tableProps.tableSize.y);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 1, GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderInput(column1Label.c_str(), "", value);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;

			return b_changed;
		}

		bool RenderFloatTableColumns(TableProps tableProps, float& value, int labelIndex, int valueIndex)
		{								
			float textWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x;
			int offsetX = (tableProps.labelWidth - textWidth) * 0.5f;

			ImGui::TableSetColumnIndex(labelIndex);		
			if (tableProps.labelColor != "")				
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(tableProps.labelColor));	
			ImGui::SameLine(0,0);
			RenderVerticalSeparator(tableProps.b_vertSeperator);
			ImGui::AlignTextToFramePadding();	
			if (offsetX > 0.0f)		
				GuiCore::MoveScreenCursor(offsetX, 0);
			ImGui::Text("%s", tableProps.label.c_str());	
			ImGui::SameLine(0,6);
			RenderVerticalSeparator(tableProps.b_vertSeperator);				
			ImGui::TableSetColumnIndex(valueIndex);	
			if (tableProps.valueColor != "")						
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(tableProps.valueColor));	
			return GuiCore::RenderDragFloat(tableProps.ID.c_str(), 0, value, tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax);
		}

		bool RenderVector2Table(TableProps tableProps, Vector2& vec2)
		{						
			bool b_changed = false;	
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;					

			if (tableProps.valueLabelColors.size() == 0)
				tableProps.valueLabelColors = { "transformXBGLight", "transformYBGLight" };
			
			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
			std::string column2Label = tableProps.ID + std::to_string(2);
						
			Vector2 regionAvailable = tableProps.tableSize.x != 0 ? tableProps.tableSize : ImGui::GetContentRegionAvail();						
			if (tableProps.labelWidth == 0) 
			{
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
				if (tableProps.labelWidth < regionAvailable.x / 3.0f)
				{
					tableProps.labelWidth = regionAvailable.x / 3.0f;
				}
			}

			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);
			RenderVerticalSeparator(tableProps.b_vertSeperator);		

			std::vector<float> widths { 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
						
			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 4, GuiCore::tableFlags, Vector2(regionAvailable.x - tableProps.labelWidth, 0), widths))
			{
				ImGui::TableNextRow();			
				TableProps floatColumn1Props = TableProps(column1Label, "X", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[0], valueColor, 16);				
				TableProps floatColumn2Props = TableProps(column2Label, "Y", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[1], valueColor, 16);												

				b_changed |= RenderFloatTableColumns(floatColumn1Props, vec2.x, 0, 1);
				b_changed |= RenderFloatTableColumns(floatColumn2Props, vec2.y, 2, 3);
				GuiCore::PopTable();
			}
			
			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;	

			return b_changed;
		}

		bool RenderVector3Table(TableProps tableProps, Vector3& vec3)
		{				
			bool b_changed = false;	
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;			

			if (tableProps.valueLabelColors.size() == 0)
				tableProps.valueLabelColors = { "transformXBGLight", "transformYBGLight", "transformZBGLight" };

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
			std::string column2Label = tableProps.ID + std::to_string(2);
			std::string column3Label = tableProps.ID + std::to_string(3);

			Vector2 regionAvailable = tableProps.tableSize.x != 0 ? tableProps.tableSize : ImGui::GetContentRegionAvail();						
			if (tableProps.labelWidth == 0) 
			{
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
				if (tableProps.labelWidth < regionAvailable.x / 4.0f)
				{
					tableProps.labelWidth = regionAvailable.x / 4.0f;
				}
			}

			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(labelTableProps);

			ImGui::SameLine(0,0);
			RenderVerticalSeparator(tableProps.b_vertSeperator);

			std::vector<float> widths { 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			
			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 6, GuiCore::tableFlags, Vector2(regionAvailable.x - tableProps.labelWidth, 0), widths))
			{
				ImGui::TableNextRow();					
				TableProps floatColumn1Props = TableProps(column1Label, "X", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[0], valueColor, 16);				
				TableProps floatColumn2Props = TableProps(column2Label, "Y", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[1], valueColor, 16);												
				TableProps floatColumn3Props = TableProps(column3Label, "Z", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[2], valueColor, 16);												
				b_changed |= RenderFloatTableColumns(floatColumn1Props, vec3.x, 0, 1);
				b_changed |= RenderFloatTableColumns(floatColumn2Props, vec3.y, 2, 3);
				b_changed |= RenderFloatTableColumns(floatColumn3Props, vec3.z, 4, 5);
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;

			return b_changed;
		}

		bool RenderVector4Table(TableProps tableProps, Vector4& vec4)
		{			
			bool b_changed = false;	
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;			

			if (tableProps.valueLabelColors.size() == 0)
				tableProps.valueLabelColors = { "transformXBGLight", "transformYBGLight", "transformZBGLight", "transformWBGLight" };

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
			std::string column2Label = tableProps.ID + std::to_string(2);
			std::string column3Label = tableProps.ID + std::to_string(3);
			std::string column4Label = tableProps.ID + std::to_string(4);

			Vector2 regionAvailable = tableProps.tableSize.x != 0 ? tableProps.tableSize : ImGui::GetContentRegionAvail();						
			if (tableProps.labelWidth == 0) 
			{
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
				if (tableProps.labelWidth < regionAvailable.x / 5.0f)
				{
					tableProps.labelWidth = regionAvailable.x / 5.0f;
				}
			}
						
			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);
			RenderVerticalSeparator(tableProps.b_vertSeperator);

			std::vector<float> widths { 16, 0, 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";
			
			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 8, GuiCore::tableFlags, Vector2(regionAvailable.x - tableProps.labelWidth, 0), widths))
			{
				ImGui::TableNextRow();			
				TableProps floatColumn1Props = TableProps(column1Label, "X", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[0], valueColor, 16);				
				TableProps floatColumn2Props = TableProps(column2Label, "Y", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[1], valueColor, 16);												
				TableProps floatColumn3Props = TableProps(column3Label, "Z", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[2], valueColor, 16);	
				TableProps floatColumn4Props = TableProps(column4Label, "W", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[3], valueColor, 16);	
				b_changed |= RenderFloatTableColumns(floatColumn1Props, vec4.x, 0, 1);
				b_changed |= RenderFloatTableColumns(floatColumn2Props, vec4.y, 2, 3);
				b_changed |= RenderFloatTableColumns(floatColumn3Props, vec4.z, 4, 5);
				b_changed |= RenderFloatTableColumns(floatColumn4Props, vec4.w, 6, 7);
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;
			
			return b_changed;
		}
		
		bool RenderFloatTable(TableProps tableProps, float& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;			

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
						
			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableProps.tableSize.x / 2.0f ? tableProps.tableSize.x / 2.0f : ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps.b_vertSeperator);

			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableProps.tableSize = Vector2((tableProps.tableSize.x - tableProps.labelWidth), tableProps.tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 1, GuiCore::tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderDragFloat(column1Label.c_str(), 0, value, tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;

			return b_changed;
		}

		bool RenderDoubleTable(TableProps tableProps, double& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;			

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
						
			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableProps.tableSize.x / 2 ? tableProps.tableSize.x / 2 : ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps.b_vertSeperator);

			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableProps.tableSize = Vector2((tableProps.tableSize.x - tableProps.labelWidth), tableProps.tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 1, GuiCore::tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderDragDouble(column1Label.c_str(), 0, value, tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;

			return b_changed;
		}

		bool RenderInt32Table(TableProps tableProps, int& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;			

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
						
			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableProps.tableSize.x / 2 ? tableProps.tableSize.x / 2 : ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps.b_vertSeperator);

			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableProps.tableSize = Vector2((tableProps.tableSize.x - tableProps.labelWidth), tableProps.tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 1, GuiCore::tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderDragInt(column1Label.c_str(), 0, value, tableProps.intIncrement, tableProps.intMin, tableProps.intMax);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;

			return b_changed;
		}

		bool RenderInt64Table(TableProps tableProps, long& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;			

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
						
			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableProps.tableSize.x / 2 ? tableProps.tableSize.x / 2 : ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps.b_vertSeperator);

			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableProps.tableSize = Vector2((tableProps.tableSize.x - tableProps.labelWidth), tableProps.tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 1, GuiCore::tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = GuiCore::RenderDragLong(column1Label.c_str(), 0, value, tableProps.intIncrement, tableProps.intMin, tableProps.intMax);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;

			return b_changed;
		}

		bool RenderBoolTable(TableProps tableProps, bool& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;			

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
						
			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableProps.tableSize.x / 2 ? tableProps.tableSize.x / 2 : ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps.b_vertSeperator);

			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;
			else
			 	tableProps.tableSize = Vector2((tableProps.tableSize.x - tableProps.labelWidth), tableProps.tableSize.y);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			std::vector<std::string> trueFalse = { "false", "true" };
			int currentBool = value ? 1 : 0;
			
			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 1, GuiCore::tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				
				b_changed = GuiCore::RenderCombo(column1Label.c_str(), value ? "true" : "false", trueFalse, currentBool, tableProps.tableSize.x);
				
				if (b_changed)
					value = (bool)currentBool;	

				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;

			return b_changed;
		}

		void RenderTextTable(TableProps tableProps, std::vector<std::string> values)
		{
			if (values.size() < 1)
				return;

			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;			

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(5, 4));	
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);	
			ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
			float tableWidth = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableWidth / 2 ? tableWidth / 2 : ImGui::CalcTextSize(values[0].c_str()).x + 10;

			TableProps labelTableProps = TableProps("##labelTable" + tableProps.ID, tableProps.label);
			labelTableProps.labelWidth = tableProps.labelWidth;
			labelTableProps.b_light = b_light;
			labelTableProps.b_lightSet = tableProps.b_lightSet;
			labelTableProps.flags = flags;
			RenderLabelTable(labelTableProps);
			
			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps.b_vertSeperator);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";				
			if (FL::GuiCore::PushTable(tableProps.ID, values.size(), flags))
			{			
				ImGui::TableNextRow();	
				ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("noEditTableText"));				
				for (int i = 0; i < values.size(); i++)
				{					
					ImGui::TableSetColumnIndex(i);
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(Assets::assetManager.GetColor(valueColor)));
					MoveScreenCursor(5, 0);
					ImGui::Text("%s", values[i].c_str());
				}					
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				ImGui::PopStyleColor();			
				FL::GuiCore::PopTable();
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;			
		}

		bool RenderComboTable(TableProps tableProps, std::string displayedValue, std::vector<std::string> options, int& currentOption)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
						
			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableProps.tableSize.x / 2 ? tableProps.tableSize.x / 2 : ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(labelTableProps);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps.b_vertSeperator);

			Vector2 innerTableSize = Vector2((tableProps.tableSize.x - tableProps.labelWidth), tableProps.tableSize.y);
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			

			if (GuiCore::PushTable("##" + tableProps.ID + "Table", 1, GuiCore::tableFlags, innerTableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				
				b_changed = GuiCore::RenderCombo(column1Label.c_str(), displayedValue, options, currentOption, innerTableSize.x);

				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				GuiCore::PopTable();
			}

			FL::GuiCore::MoveScreenCursor(0, -4);

			if (!tableProps.b_lightSet)
				b_currentTableLight = !b_currentTableLight;

			return b_changed;
		}

		void RenderLuaParametersTable(std::string ID, std::string headerString, LuaManager::LuaParameterContainer& paramContainer)
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
				Vector2 tableSize = Vector2(ImGui::GetContentRegionAvail().x - trashButtonWidth);
				tableSize.y = 0.0f;						
				std::string inputElementID = "LuaScript_" + ID + std::to_string(paramCounter);		
				switch(param.type)
				{
					case LuaManager::ParameterType_String: GuiCore::RenderStringTable(TableProps("##String" + inputElementID, param.name, tableSize), param.p_string); break;
					case LuaManager::ParameterType_Int32:  GuiCore::RenderInt32Table(TableProps("##Int32" + inputElementID, param.name, tableSize), param.p_int32); break;
					case LuaManager::ParameterType_Int64:  GuiCore::RenderInt64Table(TableProps("##Int64" + inputElementID, param.name, tableSize), param.p_int64); break;
					case LuaManager::ParameterType_Float:  GuiCore::RenderFloatTable(TableProps("##Float" + inputElementID, param.name, tableSize), param.p_float); break;
					case LuaManager::ParameterType_Double: GuiCore::RenderDoubleTable(TableProps("##Double" + inputElementID, param.name, tableSize), param.p_double); break;
					case LuaManager::ParameterType_Bool:   GuiCore::RenderBoolTable(TableProps("##Bool" + inputElementID, param.name, tableSize), param.p_bool); break;
					case LuaManager::ParameterType_Vec2:   GuiCore::RenderVector2Table(TableProps("##Vector2" + inputElementID, param.name, tableSize), param.p_vec2); break;															
					case LuaManager::ParameterType_Vec3:   GuiCore::RenderVector3Table(TableProps("##Vector3" + inputElementID, param.name, tableSize), param.p_vec3); break;													
					case LuaManager::ParameterType_Vec4:   GuiCore::RenderVector4Table(TableProps("##Vector4" + inputElementID, param.name, tableSize), param.p_vec4); break;										
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

			RenderInvisibleButton("##DropInputdropTarget" + ID, inputStart, inputSize, true, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
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

			if (ImGui::IsItemActive())
			{
				b_mouseDownCanWarp = true;
			}

			ImGui::PopStyleColor(3);

			return b_sliderChanged;
		}

		bool RenderDragDouble(std::string ID, float width, double& value, double increment, float min, float max, std::string bgColor)
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

			bool b_sliderChanged = ImGui::DragScalar(ID.c_str(), ImGuiDataType_Double, &value, increment, &min, &max, "%.3f", ImGuiSliderFlags_AlwaysClamp);			

			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
			}

			if (ImGui::IsItemActive())
			{
				b_mouseDownCanWarp = true;
			}

			ImGui::PopStyleColor(3);

			return b_sliderChanged;
		}

		bool RenderDragInt(std::string ID, float width, int& value, int increment, int min, int max, ImGuiSliderFlags flags, std::string bgColor)
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
			
			bool b_sliderChanged = ImGui::DragInt(ID.c_str(), &value, increment, min, max, "%d", flags | ImGuiSliderFlags_AlwaysClamp);
						
			if (ImGui::IsItemHovered())
			{				
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
			}

			if (ImGui::IsItemActive())
			{
				b_mouseDownCanWarp = true;
			}

			ImGui::PopStyleColor(3);

			return b_sliderChanged;
		}

		bool RenderDragLong(std::string ID, float width, long& value, int increment, int min, int max, std::string bgColor)
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
			
			bool b_sliderChanged = ImGui::DragScalar(ID.c_str(), ImGuiDataType_S64, &value, increment, &min, &max, NULL, ImGuiSliderFlags_AlwaysClamp);			
			
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
			}

			if (ImGui::IsItemActive())
			{
				b_mouseDownCanWarp = true;
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

		void RenderSectionHeader(std::string headerText, float topPadding, float bottomPadding, std::string color, std::string separatorColor, std::string textColor)
		{
			Vector2 headerP0 = ImGui::GetCursorScreenPos();
			auto winSize = ImGui::GetWindowSize();	
			ImGui::GetWindowDrawList()->AddRectFilled({ headerP0.x, headerP0.y }, { headerP0.x + winSize.x, headerP0.y + topPadding + bottomPadding + 22 }, Assets::assetManager.GetColor32(color), 0);	
			GuiCore::RenderSeparator(0, topPadding, separatorColor);	
			ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor32(textColor));
			ImGui::Text(" %s", headerText.c_str());			
			ImGui::PopStyleColor();
			GuiCore::RenderSeparator(bottomPadding, 0, separatorColor);
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
			GuiCore::RenderMenuSeparator();
		}

		void EndToolTip()
		{		
			ImGui::EndTooltip();
		}

		void RenderToolTipText(std::string label, std::string text)
		{
			std::string newLabel = label + ": ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", text.c_str());
			GuiCore::RenderMenuSeparator();
		}

		void RenderToolTipFloat(std::string label, float data)
		{
			std::string newLabel = label + ": ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", std::to_string(data).c_str());	
			GuiCore::RenderMenuSeparator();		
		}

		void RenderToolTipLong(std::string label, long data)
		{
			std::string newLabel = label + ": ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", std::to_string(data).c_str());	
			GuiCore::RenderMenuSeparator();		
		}

		void RenderToolTipLongVector(std::string label, std::vector<long> data)
		{
			std::string newLabel = label + ": ";
			ImGui::Text("%s", newLabel.c_str());
			for (int i = 0; i < data.size(); i++)
			{
				std::string dataString = std::to_string(data[i]);
				if (i < data.size() - 1)
					dataString += ",";
				ImGui::SameLine();
				ImGui::Text("%s", dataString.c_str());
			}
			GuiCore::RenderMenuSeparator();
		}
		
		void RenderWarningText(std::string warning)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor("warningText"));
			ImGui::TextWrapped("%s", warning.c_str());
			ImGui::PopStyleColor();

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