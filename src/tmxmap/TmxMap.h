#pragma once

#include "engine/application.h"

#if 0

namespace tmx {

    class TmxMap final {
    public:
        TmxMap() = default;
        TmxMap(const TmxMap&) = default;
        ~TmxMap() = default;
    

    private:
        std::string _name;
        std::string _txmFile;
    };


    // 同时只会渲染一个场景，
    // 但可能会出现同时存在多个场景的情况
    // 如从从院子进入某个房间，再退回院子，此时没必要每次都销毁院子场景

    class TmxMapManager final 
    {
        using TmxMapUniquePtr = std::unique_ptr<TmxMap>;
        using TmxMapMap = std::unordered_map<std::string, TmxMapUniquePtr>;

    public:
        TmxMapManager() = delete;
        TmxMapManager(engine::Application& render);
        TmxMapManager(TmxMapManager&&) = delete;
        TmxMapManager(const TmxMapManager&) = delete;
        ~TmxMapManager() = default;

        TmxMap* load(const std::string& name, const std::string& path);
        bool unload(const std::string& name);

        bool draw(const std::string& name);

    private:
        engine::Renderer& _render;
        std::string _mapFile;

        TmxMapMap _maps;
    };
}

#endif