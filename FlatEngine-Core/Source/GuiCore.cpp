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
		float texturePixelsPerGridSpace = 64.0f;

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
			style.DisabledAlpha = 0.3f;
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

		void WarpMouseTo(Vector2 warpTo)
		{
			SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), warpTo.x, warpTo.y);
		}
		
		void CalculateMouseDelta()
		{
			if (!b_mouseDownCanWarp)
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

		// Grabbed from Imgui file to edit directly to use ImGui's own FramePadding var as menu padding.
		bool IsRootOfOpenMenuSet()
		{
			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = g.CurrentWindow;
			if ((g.OpenPopupStack.Size <= g.BeginPopupStack.Size) || (window->Flags & ImGuiWindowFlags_ChildMenu))
				return false;

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
			ImGui::BeginGroup();
			ImGui::PushID("##MenuBar");

			const float border_top = ImMax(IM_ROUND(window->WindowBorderSize * 0.5f - window->TitleBarHeight), 0.0f);
			const float border_half = IM_ROUND(window->WindowBorderSize * 0.5f);
			ImRect bar_rect = window->MenuBarRect();
			bar_rect.Max.y += ImGui::GetStyle().CellPadding.y * 2;
			ImRect clip_rect(ImFloor(bar_rect.Min.x + border_half), ImFloor(bar_rect.Min.y + border_top), ImFloor(ImMax(bar_rect.Min.x, bar_rect.Max.x - ImMax(window->WindowRounding, border_half))), ImFloor(bar_rect.Max.y));
			clip_rect.ClipWith(window->OuterRectClipped);
			ImGui::PushClipRect(clip_rect.Min, clip_rect.Max, false);

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
			ImGui::SetCurrentViewport(NULL, viewport);
			
			ImVec2 cellPadding = ImGui::GetStyle().CellPadding;
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
			g.CurrentWindow->Flags &= ~ImGuiWindowFlags_NoSavedSettings;
			BeginMenuBar();
			return is_open;
		}
		void EndMainMenuBar()
		{
			ImGuiContext& g = *GImGui;
			if (!g.CurrentWindow->DC.MenuBarAppending)
			{
				IM_ASSERT_USER_ERROR(0, "Calling EndMainMenuBar() not from a menu-bar!");
				return;
			}
			ImGui::EndMenuBar();
			g.CurrentWindow->Flags |= ImGuiWindowFlags_NoSavedSettings;
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

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_ChildMenu | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;
			if (window->Flags & ImGuiWindowFlags_ChildMenu)
				window_flags |= ImGuiWindowFlags_ChildWindow;

			if (g.MenusIdSubmittedThisFrame.contains(id))
			{
				if (menu_is_open)
					menu_is_open = ImGui::BeginPopupMenuEx(id, label, window_flags);
				else
					g.NextWindowData.ClearFlags();
				return menu_is_open;
			}
			
			g.MenusIdSubmittedThisFrame.push_back(id);		
			const bool menuset_is_open = IsRootOfOpenMenuSet();
			if (menuset_is_open)
				ImGui::PushItemFlag(ImGuiItemFlags_NoWindowHoverableCheck, true);

			ImVec2 popup_pos, pos = window->DC.CursorPos;
			ImGui::PushID(label);
			if (!enabled)
				ImGui::BeginDisabled();
			const ImGuiMenuColumns* offsets = &window->DC.MenuColumns;
			bool pressed;

			const ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_NoHoldingActiveID | ImGuiSelectableFlags_NoSetKeyOwner | ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_NoAutoClosePopups;
			if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
			{
				popup_pos = ImVec2(pos.x - 1.0f - IM_TRUNC(style.ItemSpacing.x * 0.5f), pos.y + label_size.y - style.FramePadding.y + window->MenuBarHeight);
				window->DC.CursorPos.x += IM_TRUNC(style.ItemSpacing.x * 0.5f);
				float w = label_size.x;
				ImVec2 text_pos(window->DC.CursorPos.x + cellPadding.x + offsets->OffsetLabel, window->DC.CursorPos.y + cellPadding.y + window->DC.CurrLineTextBaseOffset);
				pressed = ImGui::Selectable("", menu_is_open, selectable_flags, ImVec2(w, label_size.y));
				ImGui::LogSetNextTextDecoration("[", "]");
				ImGui::RenderText(text_pos, label);
				window->DC.CursorPos.x += IM_TRUNC(style.ItemSpacing.x * (-1.0f + 0.5f));
			}
			else
			{
				popup_pos = ImVec2(pos.x, pos.y - style.WindowPadding.y);
				float icon_w = (icon && icon[0]) ? ImGui::CalcTextSize(icon, NULL).x : 0.0f;
				float checkmark_w = IM_TRUNC(g.FontSize * 1.20f);
				float min_w = window->DC.MenuColumns.DeclColumns(icon_w, label_size.x, 0.0f, checkmark_w);
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
			if (window->DC.LayoutType == ImGuiLayoutType_Vertical)
			{
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
					const float pad_farmost_h = ImClamp(ImFabs(ta.x - tb.x) * 0.30f, ref_unit * 0.5f, ref_unit * 2.5f);
					ta.x += child_dir * -0.5f;
					tb.x += child_dir * ref_unit;
					tc.x += child_dir * ref_unit;
					tb.y = ta.y + ImMax((tb.y - pad_farmost_h) - ta.y, -ref_unit * 8.0f); 
					tc.y = ta.y + ImMin((tc.y + pad_farmost_h) - ta.y, +ref_unit * 8.0f);
					moving_toward_child_menu = ImTriangleContainsPoint(ta, tb, tc, g.IO.MousePos);		 
				}

				if (menu_is_open && !hovered && g.HoveredWindow == window && !moving_toward_child_menu && !g.NavHighlightItemUnderNav && g.ActiveId == 0)
					want_close = true;

				if (!menu_is_open && pressed)
					want_open = true;
				else if (!menu_is_open && hovered && !moving_toward_child_menu)
					want_open = true;
				else if (!menu_is_open && hovered && g.HoveredIdTimer >= 0.30f && g.MouseStationaryTimer >= 0.30f) 
					want_open = true;
				if (g.NavId == id && g.NavMoveDir == ImGuiDir_Right)
				{
					want_open = want_open_nav_init = true;
					ImGui::NavMoveRequestCancel();
					ImGui::SetNavCursorVisibleAfterMove();
				}
			}
			else
			{
				if (menu_is_open && pressed && menuset_is_open)
				{
					want_close = true;
					want_open = menu_is_open = false;
				}
				else if (pressed || (hovered && menuset_is_open && !menu_is_open)) 
				{
					want_open = true;
				}
				else if (g.NavId == id && g.NavMoveDir == ImGuiDir_Down)
				{
					want_open = true;
					ImGui::NavMoveRequestCancel();
				}
			}

			if (!enabled) 
				want_close = true;
			if (want_close && ImGui::IsPopupOpen(id, ImGuiPopupFlags_None))
				ImGui::ClosePopupToLevel(g.BeginPopupStack.Size, true);

			IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Openable | (menu_is_open ? ImGuiItemStatusFlags_Opened : 0));
			ImGui::PopID();

			if (want_open && !menu_is_open && g.OpenPopupStack.Size > g.BeginPopupStack.Size)
			{
				ImGui::OpenPopup(label);
			}
			else if (want_open)
			{
				menu_is_open = true;
				ImGui::OpenPopup(label, ImGuiPopupFlags_NoReopen);
			}

			if (menu_is_open)
			{
				ImGuiLastItemData last_item_in_parent = g.LastItemData;
				ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Always);                  
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, style.PopupRounding); 
				menu_is_open = ImGui::BeginPopupMenuEx(id, label, window_flags); 
				ImGui::PopStyleVar();
				if (menu_is_open)
				{
					if (want_open && want_open_nav_init && !g.NavInitRequest)
					{
						ImGui::FocusWindow(g.CurrentWindow, ImGuiFocusRequestFlags_UnlessBelowModal);
						ImGui::NavInitWindow(g.CurrentWindow, false);
					}

					g.LastItemData = last_item_in_parent;
					if (g.HoveredWindow == window)
						g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
				}
			}
			else
			{
				g.NextWindowData.ClearFlags();
			}

			return menu_is_open;
		}
		bool BeginMenu(const char* label, bool enabled)
		{
			return BeginMenuEx(label, NULL, enabled);
		}
		void EndMenu()
		{
			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = g.CurrentWindow;
			IM_ASSERT(window->Flags & ImGuiWindowFlags_Popup);  
			ImGuiWindow* parent_window = window->ParentWindow;  
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

			const bool menuset_is_open = IsRootOfOpenMenuSet();
			if (menuset_is_open)
				ImGui::PushItemFlag(ImGuiItemFlags_NoWindowHoverableCheck, true);

			bool pressed;
			ImGui::PushID(label);
			if (!enabled)
				ImGui::BeginDisabled();

			const ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SelectOnRelease | ImGuiSelectableFlags_NoSetKeyOwner | ImGuiSelectableFlags_SetNavIdOnHover;
			const ImGuiMenuColumns* offsets = &window->DC.MenuColumns;
			if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
			{
				float w = label_size.x;
				window->DC.CursorPos.x += IM_TRUNC(style.ItemSpacing.x * 0.5f);
				ImVec2 text_pos(window->DC.CursorPos.x + offsets->OffsetLabel, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
				pressed = ImGui::Selectable("", selected, selectable_flags, ImVec2(w, 0.0f));
				if (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_Visible)
					ImGui::RenderText(text_pos, label);
				window->DC.CursorPos.x += IM_TRUNC(style.ItemSpacing.x * (-1.0f + 0.5f)); 
			}
			else
			{
				float icon_w = (icon && icon[0]) ? ImGui::CalcTextSize(icon, NULL).x : 0.0f;
				float shortcut_w = (shortcut && shortcut[0]) ? ImGui::CalcTextSize(shortcut, NULL).x : 0.0f;
				float checkmark_w = IM_TRUNC(g.FontSize * 1.20f);
				float min_w = window->DC.MenuColumns.DeclColumns(icon_w, label_size.x, shortcut_w, checkmark_w); 
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
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4.5));
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

		void PopTable()
		{
			ImGui::EndTable();
			PopTableStyles();
		}

		// Newer table functions		
		void RenderTableBorders(TableProps tableProps, Vector2 labelTopStart)
		{
			Vector2 valueTopStart = labelTopStart + Vector2(tableProps.labelWidth - 1, 0);			
			Vector2 valueBottomStart = valueTopStart + Vector2(1, TABLE_HEIGHT - 1);
			
			if (tableProps.b_topValueBorder)
				ImGui::GetWindowDrawList()->AddLine(valueTopStart, Vector2(valueTopStart.x + tableProps.tableSize.x + 1, valueTopStart.y), Assets::assetManager.GetColor32(tableProps.valueBorderTop), 1.0f);				
			if (tableProps.b_bottomValueBorder)
				ImGui::GetWindowDrawList()->AddLine(valueBottomStart, Vector2(valueBottomStart.x + tableProps.tableSize.x - 1, valueBottomStart.y), Assets::assetManager.GetColor32(tableProps.valueBorderBottom), 1.0f);	
			if (tableProps.b_rightBorder)
				ImGui::GetWindowDrawList()->AddLine(valueTopStart + Vector2(tableProps.tableSize.x, 0), valueBottomStart + Vector2(tableProps.tableSize.x - 1, 1), Assets::assetManager.GetColor32(tableProps.borderRight), 1.0f);	
		}

		void RenderLabelTable(TableProps tableProps)
		{
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;

			Vector4 color = Assets::assetManager.GetColor(tableProps.labelColor);
			if (!b_light)
				color *= 0.75f;

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();			
			Vector2 labelBottomStart = labelTopStart + Vector2(0, TABLE_HEIGHT - 1);			
					
			if (PushTable(tableProps.ID, 1, tableProps.flags, Vector2(tableProps.labelWidth, 0)))			
			{						 	
				ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor(tableProps.labelTextColor));
				
				ImGui::TableNextRow(0, TABLE_HEIGHT);
				ImGui::TableSetColumnIndex(0);						
				MoveScreenCursor(4, 4);		
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(color));
				ImGui::Text("%s", tableProps.label.c_str());	
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();

				ImGui::PopStyleColor();				
				PopTable();
			}

			if (tableProps.b_leftBorder)
				ImGui::GetWindowDrawList()->AddLine(labelTopStart, labelBottomStart + Vector2(0, 1), Assets::assetManager.GetColor32(tableProps.borderLeft), 1.0f);	
			if (tableProps.b_topLabelBorder)
				ImGui::GetWindowDrawList()->AddLine(labelTopStart, Vector2(labelTopStart.x + tableProps.labelWidth, labelTopStart.y), Assets::assetManager.GetColor32(tableProps.labelBorderTop), 1.0f);	
			if (tableProps.b_bottomLabelBorder)
				ImGui::GetWindowDrawList()->AddLine(labelBottomStart + Vector2(1, 0), Vector2(labelBottomStart.x + tableProps.labelWidth - 1, labelBottomStart.y), Assets::assetManager.GetColor32(tableProps.labelBorderBottom), 1.0f);	
		}

		void RenderVerticalSeparator(TableProps tableProps)
		{
			if (tableProps.b_vertSeperator)
			{
				Vector2 p0 = Vector2(ImGui::GetCursorScreenPos().x - 1, ImGui::GetCursorScreenPos().y + 1);			
				Vector2 p1 = Vector2(p0.x, p0.y + TABLE_HEIGHT - 1);
				ImGui::GetWindowDrawList()->AddLine(p0, p1, Assets::assetManager.GetColor32(tableProps.verticalSeparator), 1.0f);				
			}
		}

		bool RenderFloatTableColumns(TableProps tableProps, float& value, int labelIndex, int valueIndex)
		{								
			float textWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x;
			int offsetX = (tableProps.labelWidth - textWidth) * 0.5f;

			ImGui::TableSetColumnIndex(labelIndex);		
			if (tableProps.labelColor != "")				
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(tableProps.labelColor));	
			ImGui::SameLine(0,0);
			RenderVerticalSeparator(tableProps);
			ImGui::AlignTextToFramePadding();	
			if (offsetX > 0.0f)		
				MoveScreenCursor(offsetX, 0);
			ImGui::Text("%s", tableProps.label.c_str());	
			ImGui::SameLine(0,6);
			RenderVerticalSeparator(tableProps);				
			ImGui::TableSetColumnIndex(valueIndex);	
			if (tableProps.valueColor != "")						
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(tableProps.valueColor));	
			return RenderDragFloat(tableProps.ID.c_str(), 0, value, tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax);
		}

		bool RenderInput(std::string ID, std::string& value, float inputWidth)
		{
			bool b_editedInput = false;
			char newPath[1024] = {};

			#ifdef _WINDOWS
				strcpy_s(newPath, value.c_str());
			#elif _LINUX
				strcpy(newPath, value.c_str());
			#endif

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4.5));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, Assets::assetManager.GetColor("input"));
			ImGui::SetNextItemWidth(inputWidth != 0 ? inputWidth : ImGui::GetContentRegionAvail().x);
			b_editedInput = ImGui::InputText(ID.c_str(), newPath, IM_ARRAYSIZE(newPath));			
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			if (newPath[0] != '\0')
			{
				value = newPath;
			}
			return b_editedInput;
		}

		bool RenderStringTable(TableProps tableProps, std::string& value)
		{
			bool b_changed = false;	
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;	

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
						
			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableProps.tableSize.x / 2 ? tableProps.tableSize.x / 2 : ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;

			TableProps labelTableProps = TableProps(tableProps, column0Label, tableProps.label);					
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps);

			tableProps.tableSize.x -= tableProps.labelWidth;
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (PushTable("##" + tableProps.ID + "Table", 1, tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow(TABLE_HEIGHT);	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = RenderInput(column1Label.c_str(), value);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				PopTable();
			}

			tableProps.valueBorderTop = "inputBorderTop";
			tableProps.valueBorderBottom = "inputBorderBottom";
			tableProps.verticalSeparator = "inputSeparator";
			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;

			return b_changed;
		}


		bool RenderVector2Table(TableProps tableProps, Vector2& vec2)
		{						
			bool b_changed = false;	
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;					

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();

			if (tableProps.valueLabelColors.size() == 0)
				tableProps.valueLabelColors = { "transformXBGLight", "transformYBGLight" };
			
			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
			std::string column2Label = tableProps.ID + std::to_string(2);					

			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
			{
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
				if (tableProps.labelWidth < tableProps.tableSize.x / 3.0f)
				{
					tableProps.labelWidth = tableProps.tableSize.x / 3.0f;
				}
			}

			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);
			RenderVerticalSeparator(tableProps);		

			std::vector<float> widths { 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			

			tableProps.tableSize.x -= tableProps.labelWidth;
			if (PushTable("##" + tableProps.ID + "Table", 4, tableFlags, tableProps.tableSize, widths))
			{
				ImGui::TableNextRow();			
				TableProps floatColumn1Props = TableProps(column1Label, "X", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[0], valueColor, 16);				
				TableProps floatColumn2Props = TableProps(column2Label, "Y", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[1], valueColor, 16);												

				b_changed |= RenderFloatTableColumns(floatColumn1Props, vec2.x, 0, 1);
				b_changed |= RenderFloatTableColumns(floatColumn2Props, vec2.y, 2, 3);
				PopTable();
			}
			
			tableProps.valueBorderTop = "vectorTableValueTopBorder";
			tableProps.valueBorderBottom = "vectorTableValueBottomBorder";
			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;	

			return b_changed;
		}

		bool RenderVector3Table(TableProps tableProps, Vector3& vec3)
		{				
			bool b_changed = false;	
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;						

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();

			if (tableProps.valueLabelColors.size() == 0)
				tableProps.valueLabelColors = { "transformXBGLight", "transformYBGLight", "transformZBGLight" };

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
			std::string column2Label = tableProps.ID + std::to_string(2);
			std::string column3Label = tableProps.ID + std::to_string(3);

			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;
			
			if (tableProps.labelWidth == 0) 
			{
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
				if (tableProps.labelWidth < tableProps.tableSize.x / 4.0f)
				{
					tableProps.labelWidth = tableProps.tableSize.x / 4.0f;
				}
			}

			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(labelTableProps);

			ImGui::SameLine(0,0);
			RenderVerticalSeparator(tableProps);

			std::vector<float> widths { 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			
			tableProps.tableSize.x -= tableProps.labelWidth;
			if (PushTable("##" + tableProps.ID + "Table", 6, tableFlags, tableProps.tableSize, widths))
			{
				ImGui::TableNextRow();					
				TableProps floatColumn1Props = TableProps(column1Label, "X", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[0], valueColor, 16);				
				TableProps floatColumn2Props = TableProps(column2Label, "Y", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[1], valueColor, 16);												
				TableProps floatColumn3Props = TableProps(column3Label, "Z", Vector2(), tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax, tableProps.valueLabelColors[2], valueColor, 16);												
				b_changed |= RenderFloatTableColumns(floatColumn1Props, vec3.x, 0, 1);
				b_changed |= RenderFloatTableColumns(floatColumn2Props, vec3.y, 2, 3);
				b_changed |= RenderFloatTableColumns(floatColumn3Props, vec3.z, 4, 5);
				PopTable();
			}

			tableProps.valueBorderTop = "vectorTableValueTopBorder";
			tableProps.valueBorderBottom = "vectorTableValueBottomBorder";
			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;

			return b_changed;
		}

		bool RenderVector4Table(TableProps tableProps, Vector4& vec4)
		{			
			bool b_changed = false;	
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;						

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();

			if (tableProps.valueLabelColors.size() == 0)
				tableProps.valueLabelColors = { "transformXBGLight", "transformYBGLight", "transformZBGLight", "transformWBGLight" };

			std::string column0Label = tableProps.ID + std::to_string(0);
			std::string column1Label = tableProps.ID + std::to_string(1);
			std::string column2Label = tableProps.ID + std::to_string(2);
			std::string column3Label = tableProps.ID + std::to_string(3);
			std::string column4Label = tableProps.ID + std::to_string(4);
			
			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
			{
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;
				if (tableProps.labelWidth < tableProps.tableSize.x / 5.0f)
				{
					tableProps.labelWidth = tableProps.tableSize.x / 5.0f;
				}
			}
						
			TableProps labelTableProps = tableProps;
			labelTableProps.ID = column0Label;
			labelTableProps.labelWidth = tableProps.labelWidth;
			RenderLabelTable(tableProps);

			ImGui::SameLine(0,0);
			RenderVerticalSeparator(tableProps);

			std::vector<float> widths { 16, 0, 16, 0, 16, 0, 16, 0 };
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";
			
			tableProps.tableSize.x -= tableProps.labelWidth;
			if (PushTable("##" + tableProps.ID + "Table", 8, tableFlags, tableProps.tableSize, widths))
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
				PopTable();
			}

			tableProps.valueBorderTop = "vectorTableValueTopBorder";
			tableProps.valueBorderBottom = "vectorTableValueBottomBorder";
			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;
			
			return b_changed;
		}
		
		bool RenderFloatTable(TableProps tableProps, float& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;						

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();	

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

			RenderVerticalSeparator(tableProps);

			tableProps.tableSize.x -= tableProps.labelWidth;
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (PushTable("##" + tableProps.ID + "Table", 1, tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = RenderDragFloat(column1Label.c_str(), 0, value, tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				PopTable();
			}

			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;

			return b_changed;
		}

		bool RenderDoubleTable(TableProps tableProps, double& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;						

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();	

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

			RenderVerticalSeparator(tableProps);

			tableProps.tableSize.x -= tableProps.labelWidth;
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (PushTable("##" + tableProps.ID + "Table", 1, tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = RenderDragDouble(column1Label.c_str(), 0, value, tableProps.floatIncrement, tableProps.floatMin, tableProps.floatMax);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				PopTable();
			}

			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;

			return b_changed;
		}

		bool RenderInt32Table(TableProps tableProps, int& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;						

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();	

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

			RenderVerticalSeparator(tableProps);

			tableProps.tableSize.x -= tableProps.labelWidth;
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (PushTable("##" + tableProps.ID + "Table", 1, tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = RenderDragInt(column1Label.c_str(), 0, value, tableProps.intIncrement, tableProps.intMin, tableProps.intMax);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				PopTable();
			}

			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;

			return b_changed;
		}

		bool RenderInt64Table(TableProps tableProps, long& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;						

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();	

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

			RenderVerticalSeparator(tableProps);

			tableProps.tableSize.x -= tableProps.labelWidth;
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			if (PushTable("##" + tableProps.ID + "Table", 1, tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				b_changed = RenderDragLong(column1Label.c_str(), 0, value, tableProps.intIncrement, tableProps.intMin, tableProps.intMax);
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				PopTable();
			}

			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;

			return b_changed;
		}

		bool RenderBoolTable(TableProps tableProps, bool& value)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;						

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();	

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

			RenderVerticalSeparator(tableProps);
			
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			std::vector<std::string> trueFalse = { "false", "true" };
			int currentBool = value ? 1 : 0;
		
			tableProps.tableSize.x -= tableProps.labelWidth;
			if (PushTable("##" + tableProps.ID + "Table", 1, tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				
				b_changed = RenderCombo(column1Label.c_str(), value ? "true" : "false", trueFalse, currentBool, tableProps.tableSize.x);
				
				if (b_changed)
					value = (bool)currentBool;	

				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				PopTable();
			}

			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;

			return b_changed;
		}

		void RenderTextTable(TableProps tableProps, std::vector<std::string> values)
		{
			if (values.size() < 1)
				return;

			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;						

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();	

			ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
			float tableWidth = ImGui::GetContentRegionAvail().x;

			if (tableProps.tableSize.x == 0)
				tableProps.tableSize.x = ImGui::GetContentRegionAvail().x;

			if (tableProps.labelWidth == 0) 
				tableProps.labelWidth = ImGui::CalcTextSize(tableProps.label.c_str()).x + 9 < tableWidth / 2 ? tableWidth / 2 : ImGui::CalcTextSize(tableProps.label.c_str()).x + 10;

			TableProps labelTableProps = TableProps(tableProps, "##labelTable" + tableProps.ID, tableProps.label);
			RenderLabelTable(labelTableProps);
			
			ImGui::SameLine(0,0);

			RenderVerticalSeparator(tableProps);

			tableProps.tableSize.x -= tableProps.labelWidth;
			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";				
			if (PushTable(tableProps.ID, values.size(), flags, tableProps.tableSize))
			{			
				ImGui::TableNextRow(0, TABLE_HEIGHT);	
				ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("noEditTableText"));				
				for (int i = 0; i < values.size(); i++)
				{					
					ImGui::TableSetColumnIndex(i);
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(Assets::assetManager.GetColor(valueColor)));
					ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor("textLight"));
					MoveScreenCursor(5, 4);
					ImGui::Text("%s", values[i].c_str());
					ImGui::PopStyleColor();
				}					
				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				ImGui::PopStyleColor();			
				PopTable();
			}

			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;			
		}

		bool RenderComboTable(TableProps tableProps, std::string displayedValue, std::vector<std::string> options, int& currentOption)
		{
			bool b_changed = false;
			bool b_light = tableProps.b_lightSet ? tableProps.b_light : b_currentTableLight;			

			Vector2 labelTopStart = ImGui::GetCursorScreenPos();	

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

			RenderVerticalSeparator(tableProps);

			std::string valueColor = b_light ? "tableCellLight" : "tableCellDark";			
			tableProps.tableSize.x -= tableProps.labelWidth;
			if (PushTable("##" + tableProps.ID + "Table", 1, tableFlags, tableProps.tableSize))
			{
				ImGui::TableNextRow();	
				ImGui::TableSetColumnIndex(0);	
				if (valueColor != "")						
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Assets::assetManager.GetColor32(valueColor));	
				
				b_changed = RenderCombo(column1Label.c_str(), displayedValue, options, currentOption, tableProps.tableSize.x);

				ImGui::PushID(tableProps.ID.c_str());
				ImGui::PopID();
				PopTable();
			}

			RenderTableBorders(tableProps, labelTopStart);
			MoveScreenCursor(0, -4);
			b_currentTableLight = !b_light;

			return b_changed;
		}

		void RenderLuaParametersTable(std::string ID, std::string headerString, LuaManager::LuaParameterContainer& paramContainer)
		{
			// Set the values for a new parameter							
			std::vector<std::string> types = { "-none-", "string", "int32", "int64", "float", "double", "bool", "vec2", "vec3", "vec4" };
			LuaManager::LuaParameter newParam = LuaManager::LuaParameter();					
			
			std::string newParamComboID = "##ParameterType-createNewLuaScript_" + ID;
			if (RenderCombo(newParamComboID, types[paramContainer.tempParameterType], types, paramContainer.tempParameterType, 80))
			{
				newParam.type = (LuaManager::ParameterType)paramContainer.tempParameterType;
			}

			ImGui::SameLine(0,0);
			RenderInput("##newScriptParamInputLuaScript_" + ID, paramContainer.tempParameterName, ImGui::GetContentRegionAvail().x - 35);			

			ImGui::SameLine(0,0);
			ImGui::BeginDisabled(paramContainer.tempParameterName == "" || paramContainer.tempParameterType == LuaManager::ParameterType_None);
			if (RenderButton("ADD##LuaScript_" + ID, Vector2(35, TABLE_HEIGHT)))
			{
				newParam.type = (LuaManager::ParameterType)paramContainer.tempParameterType;
				newParam.name = paramContainer.tempParameterName;
				paramContainer.Add(newParam);
				paramContainer.tempParameterName = "";
				paramContainer.tempParameterType = 0;
			}
			ImGui::EndDisabled();	
			
			MoveScreenCursor(0,-1);

			if (paramContainer.parameters.size())
			{	
				RenderSectionHeader(headerString, 1, 1, "sectionHeaderSecondaryBg");	
				MoveScreenCursor(0, 1);
			}

			int paramCounter = 0;
			std::string paramQueuedForDelete = "";
			for (auto paramIter = paramContainer.parameters.begin(); paramIter != paramContainer.parameters.end(); paramIter++)
			{
				LuaManager::LuaParameter& param = paramIter->second;
				int currentParamType = param.type;
				
				std::string currentParamComboID = "##ParameterTypeLuaScript_" + ID + std::to_string(paramCounter);
				if (RenderCombo(currentParamComboID, types[currentParamType], types, currentParamType, 80))
				{
					param.type = (LuaManager::ParameterType)currentParamType;
				}

				ImGui::SameLine(0,3);
				int trashButtonWidth = 22;								
				Vector2 tableSize = Vector2(ImGui::GetContentRegionAvail().x - trashButtonWidth);
				tableSize.y = 0.0f;						
				std::string inputElementID = "LuaScript_" + ID + std::to_string(paramCounter);		
				switch(param.type)
				{
					case LuaManager::ParameterType_String: RenderStringTable(TableProps("##String" + inputElementID, param.name, tableSize), param.p_string); break;
					case LuaManager::ParameterType_Int32:  RenderInt32Table(TableProps("##Int32" + inputElementID, param.name, tableSize), param.p_int32); break;
					case LuaManager::ParameterType_Int64:  RenderInt64Table(TableProps("##Int64" + inputElementID, param.name, tableSize), param.p_int64); break;
					case LuaManager::ParameterType_Float:  RenderFloatTable(TableProps("##Float" + inputElementID, param.name, tableSize), param.p_float); break;
					case LuaManager::ParameterType_Double: RenderDoubleTable(TableProps("##Double" + inputElementID, param.name, tableSize), param.p_double); break;
					case LuaManager::ParameterType_Bool:   RenderBoolTable(TableProps("##Bool" + inputElementID, param.name, tableSize), param.p_bool); break;
					case LuaManager::ParameterType_Vec2:   RenderVector2Table(TableProps("##Vector2" + inputElementID, param.name, tableSize), param.p_vec2); break;															
					case LuaManager::ParameterType_Vec3:   RenderVector3Table(TableProps("##Vector3" + inputElementID, param.name, tableSize), param.p_vec3); break;													
					case LuaManager::ParameterType_Vec4:   RenderVector4Table(TableProps("##Vector4" + inputElementID, param.name, tableSize), param.p_vec4); break;										
					default: break;
				}

				ImGui::SameLine(0, 0);

				std::string trashcanID = "##trashIcon-LuaScript_" + ID + std::to_string(paramCounter);
				if (RenderImageButton(trashcanID.c_str(), Assets::assetManager.GetTexture("trash"), Vector2(16), 0.0f, Vector2(3)))
				{
					paramQueuedForDelete = paramIter->first;
				}

				paramCounter++;
			}

			if (paramQueuedForDelete != "")
			{
				paramContainer.Remove(paramQueuedForDelete);
				paramQueuedForDelete = "";
			}

			MoveScreenCursor(0, -5);
		}

		bool RenderDropInputTable(InputProps& inputProps)
		{
			bool b_editedButton = false;
			bool b_dragTargeted = false;
			char newPath[1024];		

			#ifdef _WINDOWS
				strcpy_s(newPath, openedFileValue.c_str());
			#elif _LINUX
				strcpy(newPath, inputProps.value.c_str());
			#endif

			TableProps labelTable = TableProps("##DropInputLabelTable" + inputProps.ID, inputProps.label);
			labelTable.labelColor = "col_2";
			labelTable.labelWidth = ImGui::CalcTextSize(inputProps.label.c_str()).x + 9;
			labelTable.b_topLabelBorder = true;
			labelTable.b_bottomLabelBorder = true;
			RenderLabelTable(labelTable);
			ImGui::SameLine(0, 0);			

			RenderVerticalSeparator(labelTable);
			
			if (inputProps.inputWidth == 0) inputProps.inputWidth = ImGui::GetContentRegionAvail().x;
			if (inputProps.b_canOpenFiles)  inputProps.inputWidth -= 22;
		
			Vector2 inputStart = ImGui::GetCursorScreenPos();
			Vector2 inputSize = Vector2(inputProps.inputWidth, TABLE_HEIGHT);
			ImGui::GetWindowDrawList()->AddRectFilled(inputStart, Vector2(inputStart.x + inputSize.x, inputStart.y + inputSize.y), Assets::assetManager.GetColor32("input"), 0);			
			ImGui::GetWindowDrawList()->AddRect(inputStart, Vector2(inputStart.x + inputSize.x, inputStart.y + inputSize.y), Assets::assetManager.GetColor32("inputBorder"), 0);			
			ImGui::SetCursorScreenPos(Vector2(inputStart.x + 6, inputStart.y + 4));	
			ImGui::Text("%s", inputProps.displayValue.c_str());

			RenderInvisibleButton("##DropInputOpenFilesdropTarget" + inputProps.ID, inputStart, inputSize, true, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
	
			if (inputProps.tipMessage != "" && ImGui::IsItemHovered())			
				RenderTextToolTip(inputProps.tipMessage);			

			// Drop Target
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(inputProps.dropTargetID.c_str()))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					inputProps.droppedObjectID = *(const int*)payload->Data;
					b_dragTargeted = true;
				}
				ImGui::EndDragDropTarget();
			}

			// Right click menu
			std::string dropTargetRightClickID = "##DropInputOpenFilesdropTarget##" + inputProps.ID;
			if (ImGui::BeginPopupContextItem(dropTargetRightClickID.c_str()))
			{
				PushMenuStyles();
				if (ImGui::MenuItem("Remove reference"))
				{
					inputProps.value = "";
					inputProps.droppedObjectID = -1;
					b_dragTargeted = true;
					ImGui::CloseCurrentPopup();
				}
				PopMenuStyles();

				ImGui::EndPopup();
			}

			// Open file button
			if (inputProps.b_canOpenFiles)
			{
				ImGui::SameLine(0,0);

				std::string buttonId = inputProps.ID + "openFileButton";		
				if (RenderImageButton(buttonId.c_str(), Assets::assetManager.GetTexture("openFile"), Vector2(16), 1, Vector2(3), "buttonBorder", "openFileButtonBg", "imageButtonTint", "openFileButtonHovered", "imageButtonActive"))
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
					inputProps.value = newPath;				
			}

			// Handle file drops
			if (inputProps.dropTargetID == fileExplorerTarget && inputProps.droppedObjectID < FL::GuiCore::selectedFiles.size())
			{
				std::filesystem::path fsPath(FL::GuiCore::selectedFiles[inputProps.droppedObjectID]);
				bool b_extensionCorrect = false;
				for (std::string ext : inputProps.requiredExtensions)
				{
					if (fsPath.extension() == ext)
					{
						b_extensionCorrect = true;
						inputProps.value = fsPath.string();
						break;
					}
				}
				
				if (!b_extensionCorrect)
				{
					std::string extensions;
					for (std::string ext : inputProps.requiredExtensions)
					{
						extensions += ext + " ";
					}

					FL::Logger::log.Err("File must be of type {} to drop here.", extensions);
					return false;
				}
			}

			MoveScreenCursor(0, -4);

			return b_editedButton || b_dragTargeted;
		}

		bool RenderColorPicker(std::string ID, Vector4& color)
		{			
			bool b_changed = false;			
			float colorArray[4] = {0};
			std::string tintID = "##ColorPicker" + ID;
			b_changed = ImGui::ColorEdit4(tintID.c_str(), (float*)&colorArray);
			
			if (b_changed)
			{
				color.x = colorArray[0];
				color.y = colorArray[1];
				color.z = colorArray[2];
				color.w = colorArray[3];
			}

			return b_changed;
		}

		bool RenderCombo(std::string ID, std::string displayedValue, std::vector<std::string> options, int& currentOption, float width)
		{
			bool b_interactedWith = false;

			ImGui::SetNextItemWidth(width != 0 ? width : ImGui::GetContentRegionAvail().x);

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

			if (size.x == 0)
				size.x = ImGui::GetContentRegionAvail().x;			
			if (size.y == 0)
				size.y = TABLE_HEIGHT;			

			b_isClicked = ImGui::Button(text.c_str(), size);
		

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

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(4, 4.5));
			bool b_sliderChanged = ImGui::DragFloat(ID.c_str(), &value, increment, min, max, "%.3f", flags | ImGuiSliderFlags_AlwaysClamp);
			ImGui::PopStyleVar();

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

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(4, 4.5));
			bool b_sliderChanged = ImGui::DragScalar(ID.c_str(), ImGuiDataType_Double, &value, increment, &min, &max, "%.3f", ImGuiSliderFlags_AlwaysClamp);			
			ImGui::PopStyleVar();
			
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
			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(4, 4.5));
			bool b_sliderChanged = ImGui::DragInt(ID.c_str(), &value, increment, min, max, "%d", flags | ImGuiSliderFlags_AlwaysClamp);
			ImGui::PopStyleVar();
						
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
			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(4, 4.5));
			bool b_sliderChanged = ImGui::DragScalar(ID.c_str(), ImGuiDataType_S64, &value, increment, &min, &max, NULL, ImGuiSliderFlags_AlwaysClamp);			
			ImGui::PopStyleVar();
			
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
			RenderSeparator(0, topPadding, separatorColor);	
			ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor32(textColor));
			ImGui::Text(" %s", headerText.c_str());			
			ImGui::PopStyleColor();
			RenderSeparator(bottomPadding, 0, separatorColor);
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
			RenderMenuSeparator();
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
			RenderMenuSeparator();
		}

		void RenderToolTipFloat(std::string label, float data)
		{
			std::string newLabel = label + ": ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", std::to_string(data).c_str());	
			RenderMenuSeparator();		
		}

		void RenderToolTipLong(std::string label, long data)
		{
			std::string newLabel = label + ": ";
			ImGui::Text("%s", newLabel.c_str());
			ImGui::SameLine();
			ImGui::Text("%s", std::to_string(data).c_str());	
			RenderMenuSeparator();		
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
			RenderMenuSeparator();
		}
		
		void RenderInfoButton(std::string infoText)
		{
			RenderImageButton("InfoButton" + infoText, Assets::assetManager.GetTexture("info"), Vector2(16), 0, Vector2(3), "buttonBorder", "button");

			if (ImGui::IsItemHovered())
			{
				RenderTextToolTip(infoText);
			}

			MoveScreenCursor(0, -4);
		}
		void RenderColoredText(std::string text, std::string color)
		{
			std::string indentedText = " " + text;
			ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor(color));
			ImGui::TextWrapped("%s", indentedText.c_str());
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