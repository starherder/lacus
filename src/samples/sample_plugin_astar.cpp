#include "sample_plugin_astar.h"
#include "a_star/a_star.hpp"

namespace samples {

    void ImPathFindForm::init()
    {

    }

    void ImPathFindForm::draw()
    {
        ImGui::Begin("PathFind");
        {
            drawUI();

            drawScene();

            handleEvent();
        }
        ImGui::End();
    }

    void ImPathFindForm::drawUI()
    {
        // 操作模式选择
        ImGui::Text("operator:");
        ImGui::Separator();
        if (ImGui::RadioButton("source", _state == State::SetSource)) {
            _state = State::SetSource;
        }
        ImGui::SameLine(0, 20);
        if (ImGui::RadioButton("target", _state == State::SetTarget)) {
            _state = State::SetTarget;
        }
        ImGui::SameLine(0, 20);
        if (ImGui::RadioButton("add_block", _state == State::SetBlock)) {
            _state = State::SetBlock;
        }
        ImGui::SameLine(0, 20);
        if (ImGui::RadioButton("del_block", _state == State::DelBlock)) {
            _state = State::DelBlock;
        }

        // 启发算法选择
        ImGui::Dummy({10, 20});
        ImGui::Text("heuristic:");
        ImGui::Separator();
        if (ImGui::RadioButton("Euclidean", _heuristic == Heuristic::Euclidean)) {
            _heuristic = Heuristic::Euclidean;
        }
        ImGui::SameLine(0, 20);
        if (ImGui::RadioButton("Octagonal", _heuristic == Heuristic::Octagonal)) {
            _heuristic = Heuristic::Octagonal;
        }
        ImGui::SameLine(0, 20);
        if (ImGui::RadioButton("Manhattan", _heuristic == Heuristic::Manhattan)) {
            _heuristic = Heuristic::Manhattan;
        }


        // 属性配置
        ImGui::Dummy({ 10, 20 });
        ImGui::Text("option:");
        ImGui::Separator();
        ImGui::Checkbox("diagnal", &_diagonal);


        // 开始寻路
        ImGui::Dummy({ 10, 20 });
        ImGui::Text("find:");
        ImGui::Separator();
        if (ImGui::Button("reset")) {
            resetAll();
        }
        ImGui::SameLine(0, 20);
        if (ImGui::Button("find", { 100, 0 })) {

            findPath();
        }
    }

    void ImPathFindForm::drawScene()
    {
        auto drawList = ImGui::GetWindowDrawList();
        ImVec2 wndPos = ImGui::GetWindowPos();

        ImVec2 sceneSize{ (float)_sceneGrids.x*_gridSize.x, (float)_sceneGrids.y*_gridSize.y };

        ImVec2 sceneLT{ wndPos.x + _sceneOffset.x, wndPos.y + _sceneOffset.y };
        ImVec2 sceneRB{ sceneLT.x + sceneSize.x, sceneLT.y + sceneSize.y };


        {   // ground

            drawList->AddRectFilled(sceneLT, sceneRB, _bkColor);
        }

        {   // path
            for (auto& grid : _path) {

                Vec2f gridPos = { grid.x * _gridSize.x + _sceneOffset.x,  grid.y * _gridSize.y + _sceneOffset.y };
                ImVec2 lt = { wndPos.x + gridPos.x, wndPos.y + gridPos.y };
                ImVec2 rb = { lt.x + _gridSize.x, lt.y + _gridSize.y };
                drawList->AddRectFilled(lt, rb, _pthColor);
            }
        }

        {   // block
            for (auto& grid : _blocks) {

                Vec2f gridPos = { grid.x * _gridSize.x + _sceneOffset.x,  grid.y * _gridSize.y + _sceneOffset.y };
                ImVec2 lt = { wndPos.x + gridPos.x, wndPos.y + gridPos.y };
                ImVec2 rb = { lt.x + _gridSize.x, lt.y + _gridSize.y };
                drawList->AddRectFilled(lt, rb, _blcColor);
            }
        }

        {   // source & target

            Vec2f srcGridPos = { _source.x * _gridSize.x + _sceneOffset.x,  _source.y * _gridSize.y + _sceneOffset.y };
            Vec2f dstGridPos = { _target.x * _gridSize.x + _sceneOffset.x,  _target.y * _gridSize.y + _sceneOffset.y };

            ImVec2 srcLT = {wndPos.x+srcGridPos.x, wndPos.y+srcGridPos.y};
            ImVec2 srcRB = {srcLT.x+_gridSize.x, srcLT.y+_gridSize.y};
            drawList->AddRectFilled(srcLT, srcRB, _srcColor);

            ImVec2 dstLT = { wndPos.x + dstGridPos.x, wndPos.y+dstGridPos.y };
            ImVec2 dstRB = { dstLT.x + _gridSize.x, dstLT.y + _gridSize.y };
            drawList->AddRectFilled(dstLT, dstRB, _dstColor);
        }

        {   // grids

            for (int row = 0; row < _sceneGrids.y; row++)
            {
                ImVec2 p1{ sceneLT.x, sceneLT.y + row * _gridSize.y };
                ImVec2 p2{ sceneRB.x, sceneLT.y + row * _gridSize.y };
                drawList->AddLine(p1, p2, _lnColor);
            }

            for (int col = 0; col < _sceneGrids.x; col++)
            {
                ImVec2 p1{ sceneLT.x + col * _gridSize.x, sceneLT.y };
                ImVec2 p2{ sceneLT.x + col * _gridSize.x, sceneRB.y };
                drawList->AddLine(p1, p2, _lnColor);
            }
        }
    }

    void ImPathFindForm::handleEvent()
    {
        if (!ImGui::IsWindowHovered()){
            return;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 wndPos = ImGui::GetWindowPos();
            Vec2 mousePos = { io.MousePos.x - wndPos.x , io.MousePos.y - wndPos.y };
            Vec2 inscenePos = { mousePos.x-_sceneOffset.x, mousePos.y-_sceneOffset.y };
            Vec2i gridPos = { (int)(inscenePos.x / _gridSize.x), (int)(inscenePos.y / _gridSize.y) };
            if (gridPos.x < 0 || gridPos.x > _sceneGrids.x || gridPos.y < 0 || gridPos.y > _sceneGrids.y)
            {
                return;
            }

            spdlog::info("mouse.click: pos = ({}, {}), grid_pos = ({}, {})", mousePos.x, mousePos.y, gridPos.x, gridPos.y);

            if (_state == State::SetSource) {
                _source = gridPos;
            }

            if (_state == State::SetTarget) {
                _target = gridPos;
            }

            if (_state == State::SetBlock) {
                setBlock(gridPos);
            }

            if (_state == State::DelBlock) {
                delBlock(gridPos);
            }
        }
    }

    void ImPathFindForm::setBlock(const Vec2i& grid)
    {
        _blocks.insert(grid);
       
        spdlog::info("block size = {}", _blocks.size());
    }

    void ImPathFindForm::delBlock(const Vec2i& grid)
    {
        _blocks.erase(grid);

        spdlog::info("block size = {}", _blocks.size());
    }

    void ImPathFindForm::resetAll()
    {
        _state = State::Normal;
        _diagonal = false;

        _source = { 0, 0 };
        _target = { 10, 10 };

        _path.clear();
        _blocks.clear();
    }

    void ImPathFindForm::findPath()
    {
        _path.clear();
        _state = State::Normal;

        spdlog::info("start find pos: src = ({}, {}),  dst = ({}, {})", 
            _source.x, _source.y, _target.x, _target.y);

        AStar::Generator generator;
        generator.setWorldSize({ _sceneGrids.x, _sceneGrids.y });
        generator.setDiagonalMovement(_diagonal); // 对角线移动？
        
        for (auto& block : _blocks) {
            generator.addCollision({block.x, block.y});
        }

        if (_heuristic == Heuristic::Manhattan){
            generator.setHeuristic(AStar::Heuristic::manhattan);
        }
        else if (_heuristic == Heuristic::Octagonal){
            generator.setHeuristic(AStar::Heuristic::octagonal);
        }
        else {
            generator.setHeuristic(AStar::Heuristic::euclidean);
        }

        auto result_path = generator.findPath({ _source.x, _source.y }, { _target.x, _target.y });
        for (auto& grid : result_path) {
            _path.push_back({grid.x, grid.y});
        }
    }

    ////////////////////////////////////////////////////////////////

    void SamplePluginAstar::onInit()
    {
    }

    void SamplePluginAstar::onEnable()
    {
        imgui::ImFormManager::inst().showForm<ImPathFindForm>("ImPathFindForm");
    }

    void SamplePluginAstar::onDisable()
    {
        imgui::ImFormManager::inst().closeForm("ImPathFindForm");
    }

    void SamplePluginAstar::onClose()
    {
    }


}