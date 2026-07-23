#pragma once
#include "tools/Vector2.h"

#include <map>
#include <memory>
#include <string>
#include <vector>


namespace FlatEngine
{
	class Texture;
	
	namespace TileSetManager
	{
		class TileSet
		{
		public:
			TileSet();
			std::string GetData();

			void SetName(std::string name);
			std::string GetName();
			std::pair<Vector2, Vector2> GetIndexUVs(int index);
			void SetTexturePath(std::string texturePath);
			std::string GetTexturePath();
			void SetTileSetPath(std::string tileSetPath);
			std::string GetTileSetPath();
			std::shared_ptr<Texture> GetTexture();
			std::map<int, std::pair<Vector2, Vector2>> GetTileSet();
			void SetTileSetIndices(std::vector<int> indices);
			std::vector<int> GetTileSetIndices();
			void SetTileWidth(int width);
			void SetTileHeight(int height);
			int GetTileWidth();
			int GetTileHeight();		
			void InitializeUVs();
			void ToggleTile(int index);

		private:
			std::string m_name;
			std::string m_texturePath;
			std::string m_tileSetPath;
			std::shared_ptr<Texture> m_texture;
			std::map<int, std::pair<Vector2, Vector2>> m_allTileUVs;
			std::vector<int> m_tileSetIndices;
			int m_tileWidth;
			int m_tileHeight;
		};

		extern std::vector<TileSet> tileSets;
		extern std::string selectedTileSetToEdit;
		extern std::pair<std::string, int> tileSetAndIndexOnBrush;	

		extern void SaveTileSet(TileSet tileSet);
		extern void AddTileSet(std::string path);
		extern void InitializeTileSets();
		extern TileSet* GetTileSet(std::string tileSetName);
	}
}