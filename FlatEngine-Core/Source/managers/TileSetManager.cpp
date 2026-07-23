#include "managers/Assets.h"
#include "managers/TileSetManager.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "render/Texture.h"

#include <fstream>


namespace FlatEngine
{
    namespace TileSetManager
	{
        std::vector<TileSet> tileSets = std::vector<TileSet>();
        std::string selectedTileSetToEdit = "- none -";
        std::pair<std::string, int> tileSetAndIndexOnBrush = std::pair<std::string, int>();

        TileSet::TileSet()
        {
            m_texturePath = "";
            m_texture = std::make_shared<Texture>();
            m_allTileUVs = std::map<int, std::pair<Vector2, Vector2>>();
            m_tileSetIndices = std::vector<int>();
            m_tileWidth = 16;
            m_tileHeight = 16;
        }

        std::string TileSet::GetData()
        {
            json jsonData = {
                { "name", m_name },
                { "tileSetPath", m_tileSetPath },
                { "texturePath", m_texturePath },
                { "tileWidth", m_tileWidth },
                { "tileHeight", m_tileHeight },
            };

            std::string data = jsonData.dump();
            // Return dumped json object with required data for saving
            return data;
        }

        void TileSet::SetName(std::string name)
        {
            m_name = name;
        }

        std::string TileSet::GetName()
        {
            return m_name;
        }

        std::pair<Vector2, Vector2> TileSet::GetIndexUVs(int index)
        {
            if (m_allTileUVs.count(index) > 0)
            {
                return m_allTileUVs.at(index);
            }
            else
            {
                return std::pair<Vector2, Vector2>();
            }
        }

        void TileSet::SetTexturePath(std::string texturePath)
        {
            m_texturePath = texturePath;
            if (m_texture != nullptr)
            {
                m_texture->LoadFromFile(m_texturePath);
            }
            InitializeUVs();
        }

        std::string TileSet::GetTexturePath()
        {
            return m_texturePath;
        }

        void TileSet::SetTileSetPath(std::string tileSetPath)
        {
            m_tileSetPath = tileSetPath;
        }

        std::string TileSet::GetTileSetPath()
        {
            return m_tileSetPath;
        }

        std::shared_ptr<Texture> TileSet::GetTexture()
        {
            return m_texture;
        }

        std::map<int, std::pair<Vector2, Vector2>> TileSet::GetTileSet()
        {
            return m_allTileUVs;
        }

        void TileSet::SetTileSetIndices(std::vector<int> indices)
        {
            m_tileSetIndices = indices;
        }

        std::vector<int> TileSet::GetTileSetIndices()
        {
            return m_tileSetIndices;
        }

        void TileSet::SetTileWidth(int width)
        {
            m_tileWidth = width;
        }

        void TileSet::SetTileHeight(int height)
        {
            m_tileHeight = height;
        }

        int TileSet::GetTileWidth()
        {
            return m_tileWidth;
        }

        int TileSet::GetTileHeight()
        {
            return m_tileHeight;
        }

        void TileSet::InitializeUVs()
        {
            m_tileSetIndices.clear();
            m_allTileUVs.clear();

            if (m_tileWidth != 0 && m_tileHeight != 0 && m_texture != nullptr && m_texture->GetTexture() != nullptr)
            {
                int textureWidth = m_texture->GetWidth();
                int textureHeight = m_texture->GetHeight();
                int tilesAcross = textureWidth / m_tileWidth;
                int tilesDown = textureHeight / m_tileHeight;

                // if the desired tile dimensions are not aligned perfectly with the texture dimensions, add 1 more tile across or down for the overhang
                if (textureWidth % m_tileWidth != 0)
                {
                    tilesAcross += 1;
                }
                if (textureHeight % m_tileHeight != 0)
                {
                    tilesDown += 1;
                }

                int counter = 0;

                for (int h = 0; h < tilesAcross; h++)
                {
                    for (int v = 0; v < tilesDown; v++)
                    {
                        Vector2 uvStart = Vector2((float)(h * m_tileWidth), (float)(v * m_tileHeight));
                        Vector2 uvEnd = Vector2((float)(uvStart.x + m_tileWidth), (float)(uvStart.y + m_tileHeight));

                        // Make sure we're not saving a UV position that is outside the bounds of the SDL_Texture
                        if (textureWidth % m_tileWidth != 0 && h == tilesAcross - 1)
                        {
                            uvEnd.x -= textureWidth % m_tileWidth;
                        }
                        if (textureHeight % m_tileHeight != 0 && v == tilesDown - 1)
                        {
                            uvEnd.x -= textureHeight % m_tileHeight;
                        }

                        std::pair<Vector2, Vector2> uvs = { uvStart, uvEnd };
                        m_allTileUVs.emplace(counter, uvs);

                        counter++;
                    }
                }
            }
        }

        void TileSet::ToggleTile(int index)
        {
            for (std::vector<int>::iterator iter = m_tileSetIndices.begin(); iter != m_tileSetIndices.end(); iter++)
            {
                if ((*iter) == index)
                {
                    m_tileSetIndices.erase(iter);
                    return;
                }
            }
            
            if (m_allTileUVs.count(index) > 0)
            {
                m_tileSetIndices.push_back(index);
            }
        }


		// TileSet / TileMap Management
        void CreateNewTileSetFile(std::string fileName, std::string path)
        {
        	std::string filePath = "";		
        	TileSet tileSet = TileSet();

        	if (path == "")
        	{
        		filePath = Assets::assetManager.GetDir("projectDir") + "/tileSets" + "/" + fileName + ".tls";
        	}
        	else
        	{
        		filePath = path + "/" + fileName + ".tls";
        	}

        	tileSet.SetTileSetPath(filePath);
        	tileSet.SetName(fileName);
        	SaveTileSet(tileSet);
        	AddTileSet(filePath);
        }

        void SaveTileSet(TileSet tileSet)
        {
        	std::string texturePath = tileSet.GetTexturePath();
        	std::string filepath = tileSet.GetTileSetPath();		

        	std::ofstream fileObject;
        	std::ifstream ifstream(filepath);

        	// Delete old contents of the file
        	fileObject.open(filepath, std::ofstream::out | std::ofstream::trunc);
        	fileObject.close();

        	// Opening file in append mode
        	fileObject.open(filepath, std::ios::app);

        	std::string data = tileSet.GetData();		
        	json tileArray = json::array();

        	for (int index : tileSet.GetTileSetIndices())
        	{
        		json jsonData = {
        			{ "index", index }
        		};
                
        		std::string tileData = jsonData.dump();
        		tileArray.push_back(json::parse(tileData));
        	}
            
        	json tileSetJson = json::object({
        		{ "tileSetData", json::parse(data) },
        		{ "tiles", tileArray }
        	});

        	fileObject << tileSetJson.dump(4).c_str() << std::endl;
        	fileObject.close();
        }

        void AddTileSet(std::string path)
        {
        	TileSet tileSet = TileSet();
        	json tileSetJson = JsonHelper::LoadFileData(path);

        	if (tileSetJson != nullptr)
        	{
        		auto tileSetData = tileSetJson["tileSetData"];

        		tileSet.SetName(tileSetData["name"]);
        		tileSet.SetTileSetPath(tileSetData["tileSetPath"]);
        		tileSet.SetTexturePath(tileSetData["texturePath"]);
        		tileSet.SetTileWidth(tileSetData["tileWidth"]);
        		tileSet.SetTileHeight(tileSetData["tileHeight"]);

        		std::vector<int> indices;
        		for (auto tile : tileSetJson["tiles"])
        		{
        			int index = JsonHelper::CheckJsonInt(tile, "index", "TileSet tiles");
        			if (index != -1)
        			{
        				indices.push_back(index);
        			}
        		}
        		tileSet.SetTileSetIndices(indices);

        		tileSets.push_back(tileSet);
        	}
        }

        void InitializeTileSets()
        {
        	tileSets.clear();

        	std::vector<std::string> tileSetFiles = std::vector<std::string>();
        	tileSetFiles = FileHelper::FindAllFilesWithExtension(Assets::assetManager.GetDir("projectDir"), ".tls");

        	for (std::string path : tileSetFiles)
        	{			
        		AddTileSet(path);
        	}
        }

        TileSet* GetTileSet(std::string tileSetName) 
        {
        	for (std::vector<TileSet>::iterator iter = tileSets.begin(); iter != tileSets.end(); iter++)
        	{
        		if (iter->GetName() == tileSetName)
        		{
        			return &(*iter);
        		}			
        	}

        	return nullptr;
        }


        void DrawCircle(Vector2 center, float radius, Vector4 color, ImDrawList* drawList, float thickness, int segments)
        {
        	drawList->AddCircle(center, radius, ImGui::GetColorU32(color), segments, thickness);
        }
    }
}