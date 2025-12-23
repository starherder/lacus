#pragma once

#include "map_utils.h"


namespace game {

	struct TileAnimation 
	{
		struct Frame 
		{
			int tileId;
			int dration;
		};

		std::vector<Frame> frames;

		void load(const json& json_data);
	};

	struct MapTile 
	{
		int id;

		Properties properties;
		TileAnimation animation;

		// generatally, each tileset has only one image
		// all tiles in the tileset share the same image
		// but, Tild tool support multiple images in a tileset
		// we dont use this feature now, just ignore it.
		//std::string tile_image;
		//Vec2i tile_imageSize;
		//engine::Texture* tile_texture = nullptr;

		void load(const json& json_data);
	};

	struct TileSet 
	{
		std::string name;
		std::string type;
		std::string version;

		int firstgid;

		int margin;
		int spacing;
		int columns;
		
		int tileCount;
		Vec2i tileSize;
		std::string tileVersion;

		std::string imageFile;
		Vec2i imageSize;

		Properties properties;		

		std::map<int, MapTile> tiles;

		engine::Texture* texture = nullptr;

		void load(const json& json_data);
	};


}