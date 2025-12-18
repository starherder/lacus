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

### 12.16

- 测试tweeny、a-star库使用
