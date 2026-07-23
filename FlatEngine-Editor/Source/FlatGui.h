#pragma once

#include <string>

/*
######################################
######							######
######     Gui Declarations     ######
######                          ######
######################################
*/


namespace FlatGui
{
	extern void Init();

	extern void AddViewports();
	extern void MainMenuBar(bool& b_show);
	extern void RenderAnimator(bool& b_show);	
	extern void RenderFileExplorer(bool& b_show);
	extern void RenderHierarchy(bool& b_show);
	extern void RenderInspector(bool& b_show);
	extern void RenderKeyFrameEditor(bool& b_show);
	extern void RenderLog(bool &b_show);
	extern void RenderMappingContextEditor(bool& b_show);
	extern void RenderMaterialEditor(bool& b_show);
	extern void RenderModals();
	extern void RenderPersistantScript(bool& b_show);
	extern void RenderProfiler(bool& b_show);
	extern void RenderProjectHub(bool& b_projectSelected, std::string& projectPath);
	extern void RenderSettings(bool& b_show);
	extern void RenderTileSetEditor(bool& b_show);
	extern void RenderToolbar();
	// extern void RenderTileSetEditorTile(std::pair<int, std::pair<FL::Vector2, FL::Vector2>> tile, FL::Vector2 tileSize, FL::Vector2& scrolling, FL::Vector2 canvasP0, FL::Vector2 canvasSize, FL::Vector2& step, FL::TileSet* tileSet);
}