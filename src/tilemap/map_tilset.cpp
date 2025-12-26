#include "map_tilset.h"


namespace tilemap {

	void TileAnimation::load(const json& json_data)
	{
		for(auto& fr : json_data) {
			auto tileid = fr.value("tileid", 0);
			auto duration = fr.value("duration", 100);
			frames.emplace_back(tileid, duration);
		}
	}

	void MapTile::load(const json& json_data)
	{
		id = json_data.value("id", 0);
		if (json_data.contains("animation"))
		{
			animation.load(json_data["animation"]);
		}

		//tile_image = json_data.value("image", "");
		//tile_imageSize.x = json_data.value("imagewidth", 0);
		//tile_imageSize.y = json_data.value("imageheight", 0);

		LoaderUtils::loadProperties(json_data, properties);
	}

	void TileSet::load(const json& json_data)
	{
		name = json_data.value("name", "");
		type = json_data.value("type", "");
		version = json_data.value("version", "");

		margin = json_data.value("margin", 0);
		spacing = json_data.value("spacing", 0);
		columns = json_data.value("columns", 0);

		tileSize.x = json_data.value("tilewidth", 0);
		tileSize.y = json_data.value("tileheight", 0);
		tileCount = json_data.value("tilecount", 0);
		tileVersion = json_data.value("tiledversion", "");
	
		imageFile = json_data.value("image", "");
		imageSize.x = json_data.value("imagewidth", 0);
		imageSize.y = json_data.value("imageheight", 0);

		LoaderUtils::loadProperties(json_data, properties);

		if(json_data.contains("tiles"))
		{
			auto& js_tiles = json_data["tiles"];
			for(auto& tile : js_tiles) 
			{
				MapTile map_tile;
				map_tile.load(tile);
				tiles.insert({map_tile.id, map_tile});	
			}
		}
	}


}