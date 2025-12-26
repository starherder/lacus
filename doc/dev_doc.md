# Lacus

## 环境

vscode + vs compiler+MinGW

VSCode Plugins:  clangd, clangd-format, Cmake, Cmake Tools, CodeBuddy,

#### 代码跳转

注意vscode使用clangd插件来实现定义跳转时，只能在本目录文件之间跳转。

此时需要生成compile_commands.json才能在不同目录之间跳转，CMakeList里需要加上

```
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

然后ctrl+shift+P，CMake选择工具包里选择gcc或者clang，然后

```
mkdir build & cd build & cmake ..
```

接着可以通过选择工具包换回visualstudio的编译器

## 外部库

- SDL3 / SDL3_image / SDL3_ttf / SDL3_mixer
- spdlog
- glm
- imgui
- entt
- nlohmann-json
- tweeny
- luabridge3
- braintree
- a-star

## 计划

    场景TileMap层级

    1. 背景层(ImageLayer)，（棋盘...）：图片，装饰

    2. 地形层(TileLayer)，（土地、草地、沙漠、水面、道路...）：用于行走

    3. 前景层(ObjectLayer)，（树林、草丛、云雾、建筑...）：装饰遮挡

    4. 逻辑层(ObjectLayer)，（刷怪点、触发区、标记...）：玩法逻辑

## 日志

### 12.9 环境及框架

- 开发环境部署 VSCode + TRAE(AI)，CMake
- SDL3+spdlog+glm+imgui windows库及头文件
- render类、windows类、application类及plugin类

### 12.10 加入配置和imgui

- Config类读写json配置
- 增加ImGui的封装类，放在imform目录
- 正常显示了ImGui

### 12.11 增加材质、字体、声音的读取和渲染、播放，

- resourceManager类
- texture\font\audio类
- 需要进一步测试
- 各个模块的初始化时机需要修改确定

### 12.12

- 测试entt
- 增加摄像机

### 12.15

- 整理代码
- 增加插件管理器和界面
- 添加tweeny、a-start、braintree、luabridge库

### 12.16 测试tweeny、a-star库使用

- sample_plugin_tweeny
- sample_plugin_astar

### 12.17 测试TileEditor地图载入

- TileEditor的使用
- tmxlite 库的引入
- 测试并修改tmxlite的载入例子

### 12.18 整理代码

- 使用hashstring来作为资源的key
- vscode环境设置

### 12.19 tilemap 通过txmlite库读取

- sample_plugin_tilemap 通过tmxlite库读取tilemap

### 12.22  tilemap显示、Color类、simpleGui

- tmxlite的例子有问题，修改并显示tilemap
- 增加新的Color类、Rect类
- 增加简易的GUI

### 12.23 完善 Gui

- 增加简易layout、
- progressbar
- sliderbar
- checkbox

### 12.24 增加基于entt的寻路、运动

- entt角色信息
- entt+astar的寻路
- 调试信息图形显示

### 12.25 增加摄像机和行为树

- 添加摄像机及摄像机的运动事件
- braintree的测试例子 sample_plugin_bevtree















---  

---
