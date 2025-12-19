#include "tmx_map_loader.h"

#include "engine/application.h"


#include "tmxlite/map.hpp"


namespace tmx {

    using namespace engine;

    TmxMapLoader::TmxMapLoader(engine::Application& app) 
        : _application(app)
    {   
    }

    TmxMapLoader::~TmxMapLoader() 
    {
    }
    
    bool TmxMapLoader::load(const std::string& mapfile)
    {
        auto& textureMgr = _application.resourceManager().textureManager();
        auto& resPath = _application.resPath();
        auto mapFilePath  = resPath / mapfile;
        auto mapFileName = mapFilePath.filename();
        auto mapPath = mapFilePath.parent_path();
        auto mapRelatePath = mapPath.lexically_relative(resPath);

        auto currentPathBak = fs::current_path();

        // Tmxlite内部使用tmx文件所在位置为当前路径，所以要设置一下
        fs::current_path(mapPath);

        spdlog::info("resPath = {}, mapFilePath = {}, mapPath = {}, maprelatePath = {}", 
            resPath.string(), mapFilePath.string(), mapPath.string(), mapRelatePath.string());

        tmx::Map map;
        if (!map.load(mapFileName.string()))
        {
            spdlog::error("load file ({}) failed.", mapFilePath.string());
            return false; 
        }

        //load the textures as they're shared between layers
        std::vector<engine::Texture*> textures;
        const auto& tileSets = map.getTilesets();
        assert(!tileSets.empty());

        for (const auto& ts : tileSets)
        {
            auto imagePath = mapRelatePath / ts.getImagePath();
            auto texture = textureMgr.load(HashString(imagePath.string().c_str()));
            if(!texture)
            {
                spdlog::error("load tileset(name = {}, path = {}) failed. image_path = {}", ts.getName(), ts.getImagePath(), imagePath.string());
                continue;
            }

            textures.push_back(texture);
        }

        //load the layers
        const auto& mapLayers = map.getLayers();
        for (auto i = 0u; i < mapLayers.size(); ++i)
        {
            if (mapLayers[i]->getType() == tmx::Layer::Type::Tile)
            {
                _layers.emplace_back(std::make_unique<MapLayer>());
                _layers.back()->create(map, i, textures); //just cos we're using C++14
            }
        }

        // 将当前位置设置回来
        fs::current_path(currentPathBak);

        spdlog::info("load map OK. textures.size = {}, layer.size = {}", textures.size(), _layers.size());
        return true;
    }

    void TmxMapLoader::setPos(const engine::Vec2f& pos)
    {
        for (auto& layer : _layers)
        {
            layer->setPos(pos);
        }
    }

    void TmxMapLoader::draw()
    {
        engine::Renderer& renderer = _application.renderer();

        for (const auto& l : _layers)
        {
            l->draw(&renderer);
        }
    }
}