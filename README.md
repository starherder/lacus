# Lacus

#### 介绍

Lacus是一个基于C++20和SDL3的游戏引擎框架。

## 软件架构

- 语言：C++20
- 图形库：SDL3
- 构建系统：CMake
- 开发环境：VSCode

## 构建说明

### 环境要求

- C++20兼容的编译器（推荐GCC 10+或MSVC 2019+）
- CMake 3.20+
- SDL3库

### 构建步骤

1. 下载SDL3库并放置在`ext/SDL3`目录中
2. 配置项目：
   ```bash
   cmake -S . -B build
   ```
3. 编译项目：
   ```bash
   cmake --build build
   ```
   
### VSCode一键构建运行

1. 在VSCode中打开项目
2. 按`Ctrl+Shift+P`，选择`Tasks: Run Task`，然后选择`configure`任务
3. 再次按`Ctrl+Shift+P`，选择`Tasks: Run Task`，然后选择`build`任务
4. 按`F5`启动调试运行

## 使用说明

1. 在`src/`目录中添加你的源代码
2. 在`ext/`目录中放置外部依赖库
3. 修改`CMakeLists.txt`以适应你的项目需求

## 参与贡献

1. Fork 本仓库
2. 新建 Feat_xxx 分支
3. 提交代码
4. 新建 Pull Request