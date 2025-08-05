#pragma once
#include "Component.h"
#include "TileSet.h"
#include "Vector2.h"

#include <vector>
#include <string>
#include <map>


namespace FlatEngine
{
    struct Tile {
        Vector2 tileCoord;
        std::string tileSetName;
        SDL_Texture* tileSetTexture;
        int tileSetIndex;
        Vector2 uvStart;
        Vector2 uvEnd;
    };

    struct CollisionAreaData {
        Vector2 startCoord;
        Vector2 endCoord;
    };

    class TileMap : public Component
    {
    public:
        TileMap(long myID = -1, long parentID = -1);
        ~TileMap();
        std::string GetData();

        int GetWidth();
        int GetHeight();
        int GetTileWidth();
        int GetTileHeight();
        void SetTileWidth(int width);
        void SetTileHeight(int height);
        void SetWidth(int width);
        void SetHeight(int height);
        void SetRenderOrder(int renderOrder);
        int GetRenderOrder();
        std::string GetSelectedTileSet();
        void SetSelectedTileSet(std::string tileSet);
        std::map<std::string, std::vector<CollisionAreaData>>& GetCollisionAreas();
        //BoxCollider* AddCollisionArea(std::string label, Vector2 startCoord, Vector2 endCoord);
        void SetCollisionAreas(std::map<std::string, std::vector<CollisionAreaData>> collisionAreas);
        //void RemoveCollisionArea(std::string label, Vector2 startCoord, Vector2 endCoord);
        void SetCollisionAreaValues(std::string label, std::vector<std::pair<Vector2, Vector2>> colCoords);
        void CalculateColliderValues(std::string label);
        std::string GetSelectedCollisionArea();
        void SetSelectedCollisionArea(std::string selectedCollisionArea);
     
        void MoveTiles(std::vector<Vector2> tiles, Vector2 moveAmount);
        void MoveTile(Vector2 tileCoords, Vector2 moveAmount);
        bool ContainsCollisionAreaLabel(std::string label);
        void AddTileSet(std::string name);
        void RemoveTileSet(std::string name);
        void SetTileSets(std::vector<std::string> tileSets);
        std::vector<std::string> GetTileSets();
        std::map<int, std::map<int, Tile>> GetTiles();        
        void SetTile(Vector2 tileMapCoords, TileSet* tileSet, int tileSetIndex);
        void SetTiles(std::map<int, std::map<int, Tile>> tiles);
        void EraseTile(Vector2 tileMapCoords);

    private:
        int m_width;
        int m_height;
        int m_tileWidth;
        int m_tileHeight;
        int m_renderOrder;
        std::map<int, std::map<int, Tile>> m_tiles;
        std::map<int, std::map<int, Tile>> m_tempMoveTiles;
        std::string m_selectedTileSet;
        std::vector<std::string> m_tileSetNames;  
        std::string m_selectedCollisionArea;
        std::map<std::string, std::vector<CollisionAreaData>> m_collisionAreas;
    };
}

