#pragma once

#include "engine/application.h"
#include "imform/imform_manager.h"

#include "tweeny/tweeny.h"

namespace samples {
    using namespace engine;

    // 随便的排序函数，顺序不重要
    struct Vec2iComparator {
        bool operator()(const Vec2i& lval, const Vec2i& rval) const {
            if (lval.x == rval.x) {
                return lval.y < rval.y;
            } else {
                return lval.x < rval.x;
            }
        }
    };


    // 窗口
    class ImPathFindForm : public imgui::ImForm
    {
        enum class State {
            Normal,
            SetSource,
            SetTarget,
            SetBlock,
            DelBlock,
        };

        enum class Heuristic {
            Euclidean,
            Octagonal,
            Manhattan,
        };

    public:
        ImPathFindForm() = default;
        ~ImPathFindForm() = default;

    private:
        void init();

        void draw() override;

        void drawUI();

        void drawScene();

        void handleEvent();

        void findPath();

        void resetAll();

        void setBlock(const Vec2i& grid);
        void delBlock(const Vec2i& grid);

    private:
        const Vec2i _sceneGrids = { 20, 20};
        const Vec2i _gridSize = { 40, 40};
        const Vec2i _sceneOffset = {10, 400};

        ImColor _bkColor = { 0.5f, 0.5f, 0.5f, 1.0f };
        ImColor _lnColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        ImColor _srcColor = { 0.0f, 1.0f, 0.0f, 1.0f };
        ImColor _dstColor = { 1.0f, 0.0f, 0.0f, 1.0f };

        ImColor _blcColor = { 0.2f, 0.2f, 0.2f, 1.0f };
        ImColor _pthColor = { 0.9f, 0.9f, 0.7f, 1.0f };

        State _state = State::Normal;
        Heuristic _heuristic = Heuristic::Euclidean;

        bool _diagonal = false;

        Vec2i _source = { 0, 0 };
        Vec2i _target = { 10, 10};

        std::vector<Vec2i> _path;

        std::set<Vec2i, Vec2iComparator> _blocks;
    };



    // 测试A-star寻路库
    class SamplePluginAstar final : public engine::Plugin
    {
    public:
        SamplePluginAstar() = default;
        ~SamplePluginAstar() = default;

        const char* name() override { return "sample_astar_plugin"; }

        void onInit() override;

        void onEnable() override;

        void onDisable() override;

        void onClose() override;

    };
}