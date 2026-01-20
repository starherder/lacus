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

- ~~行为树增加xml/json配置~~
- ~~简易2d粒子系统~~
- ~~消息在场景和界面、imgui之间的传递~~
- ~~场景增加context，gameplay中每个system独立成类，system中没有数据，通用的数据在context中保存~~
- ~~npc的配置读取创建~~
- ~~painter系统优化完善、译文系统~~
- ~~物品、拾取、选择、debug~~
- ~~伤害飘字、主界面、角色控制~~
- json配置的特效系统，可配置的tween动画、粒子、alpha、色彩、大小、位置、帧动画等的组合特效
- ~~简易技能系统，表现+伤害，表现层引入tweeny，伤害公式计算使用lua配置~~
- ~~render支持的功能不多，painter表现有错误（圆角矩形），gfxpainter性能不高（批次太高）、支持的功能也少（矩形线宽没有），试着能不能把imgui中的渲染模块拆出来用~~
- ~~考察了nanovg、nanogui、nangui-sdl、imgui等库，~~
- ~~发现imgui可以在窗口之外绘制几何图形，而且也支持材质的显示，能满足所有需求，可以用来作为渲染引擎，并且支持SDL3和SDL3_GPU，并且跨平台，便于以后优化扩展，试试它吧！~~
- 新技能：冲击波（？）对一行或者周围格子角色造成伤害；光环：给周围的角色加上buf；冲刺：冲撞一条线上的角色，造成伤害；
- 开始界面、loading界面(异步加载)、主界面、场景选择界面、切换场景、放置角色、提拉角色、角色性格对应不同行为树、数据序列化及反序列化



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

### 12.12 entt及摄像机

- 测试entt
- 增加摄像机

### 12.15 插件管理器

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

### 12.26 行为树封装配置

- 行为树读取xml
- 增加行为树测试用例

### 12.29 2D粒子系统

- 引入ore项目的粒子系统
- 引入ore项目的粒子编辑器

### 12.30  整理场景和ECS模块组织

- 整理ECS场景中模式
- 增加GameContext类
- 增加巡逻NPC行为树

### 12.31 移动系统

- motionSystem
- renderSystem

### 1.1 - 1.3 tween移动、npc配置

- 使用tween实现移动功能，增加多种移动效果
- 增加NPC配置

### 1.4 NPC配置模块

- 读取NPC配置创建NPC
- 创建物品

### 1.5 自动拾取、选择、debug模块

- 根据配置创建物品
- bevnode自动拾取逻辑
- 选择对象、debug窗口


- 

### 1.15 使用ImGui的渲染模块实现painter
- 渲染基本图形
- 渲染字体、材质
- 渲染顶点数据

---

---
