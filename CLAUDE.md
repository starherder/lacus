# lacus
这是一个2D策略类游戏，主要玩法是收集卡牌和列阵战斗。 
语言使用C++ 17，以及少量的lua脚本。 
引擎主体是SDL3，使用了大量的github上的开源库，偏向简易的单个头文件的库。 
玩法层使用ecs模式的entt库。 

# 工程说明

## ext

第三方库，多数是github上的开源库，少部分有修改

## res

游戏资源，有配置、脚本、材质、音效等

## vsproject

visual stuido 工程和解决方案文件，使用vs 2022

## src

### 代码结构

- bevtree： 行为树
- effect： 特效系统
- engine： 简单的引擎，sdl3的简易封装
- game： 游戏主要代码
- imform： Dear ImGui系统的简易封装
- particle：一个简易的粒子系统
- samples：示例，主要是各个系统的测试用例
- tilemap：读取瓦片地图的系统
- ui：一个简易的UI系统
- utility：自己实现的工具库代码


## 代码规范
@.cluade/rules/style.md