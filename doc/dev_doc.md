# Lacus

## 环境

1 windows安装MSYS2
    下载地址：https://www.msys2.org/

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

    12.9 环境及框架

- 开发环境部署 VSCode + TRAE(AI)，CMake
- SDL3+spdlog+glm+imgui windows库及头文件
- render类、windows类、application类及plugin类

  12.10 加入配置和imgui
- Config类读写json配置
- 增加ImGui的封装类，放在imform目录
- 正常显示了ImGui

  12.11 增加材质、字体、声音的读取和渲染、播放，
- resourceManager类
- texture\font\audio类
- 需要进一步测试
- 各个模块的初始化时机需要修改确定

  12.12
- 测试entt
- 增加摄像机

12.15

- 整理代码
- 增加插件管理器和界面
- 添加tweeny、a-start、braintree、luabridge库
