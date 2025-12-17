#include "TmxMapDemo.h"

#include "engine/application.h"


#include "tmxlite/map.hpp"


namespace tmx {

    using namespace engine;

    TmxMapDemo::TmxMapDemo(engine::Application& app) 
        : _application(app)
    {   
    }

    TmxMapDemo::~TmxMapDemo() 
    {
    }
    
    bool TmxMapDemo::load(const std::string& mapfile)
    {
        auto& textureMgr = _application.resourceManager().textureManager();
        auto& resPath = _application.resPath();
        auto mapFilePath  = resPath / mapfile;
        auto mapFileName = mapFilePath.filename();
        auto mapPath = mapFilePath.parent_path();
        auto mapRelatePath = mapPath.lexically_relative(resPath);

        auto currentPath = fs::current_path();

        // Tmxlite模块里使用当前路径来读取文件的，所以要设置当前路径
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
        const auto& tileSets = map.getTilesets();
        assert(!tileSets.empty());
        for (const auto& ts : tileSets)
        {
            auto imagePath = mapRelatePath / ts.getImagePath();
            auto texture = textureMgr.load(ts.getName(), imagePath.string());
            if(!texture)
            {
                spdlog::error("load tileset(name = {}, path = {}) failed. image_path = {}", ts.getName(), ts.getImagePath(), imagePath.string());
                continue;
            }

            _textures.push_back(texture);
        }

        //load the layers
        const auto& mapLayers = map.getLayers();
        for (auto i = 0u; i < mapLayers.size(); ++i)
        {
            if (mapLayers[i]->getType() == tmx::Layer::Type::Tile)
            {
                _layers.emplace_back(std::make_unique<MapLayer>());
                _layers.back()->create(map, i, _textures); //just cos we're using C++14
            }
        }

        // 把路径设置回来
        fs::current_path(currentPath);

        spdlog::info("load map OK. textures.size = {}, layer.size = {}", _textures.size(), _layers.size());
        return true;
    }

    void TmxMapDemo::setPos(const engine::Vec2f& pos)
    {
        for (auto& layer : _layers)
        {
            layer->setPos(pos);
        }
    }

    void TmxMapDemo::draw()
    {
        engine::Renderer& renderer = _application.renderer();

        for (const auto& l : _layers)
        {
            l->draw(&renderer);
        }
    }
}