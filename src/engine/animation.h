#pragma once

#include "engine/wrapper.h"
#include "engine/texture.h"
#include "engine/misc.h"
#include "i_painter.h"


namespace engine
{

/// @brief 帧动画：多个材质按时间轮流播放
class Animation final
{
public:
    /// @brief 单帧数据
    struct Frame
    {
        TexTile* texture = nullptr; // 当前帧材质
        int duration = 0;           // 持续时长（毫秒）
        std::string event;          // 帧事件（可选）
    };

public:
    Animation();
    ~Animation();

    /// @brief 在指定索引插入帧
    void addFrame(int index, const Frame& frame);

    /// @brief 获取帧总数
    int frameCount() const { return (int)_frames.size(); }

    /// @brief 获取指定索引的帧
    const Frame& getFrame(int index) const;

    /// @brief 设置循环模式（0=不循环, >0=循环次数, -1=无限）
    void setLoop(int loop) { _loop = loop; }
    int loop() const { return _loop; }

    /// @brief 设置播放速率
    void setRate(float rate) { _rate = rate; }
    float rate() const { return _rate; }

    /// @brief 获取当前帧的材质
    TexTile* getCurrentTexture() const;

    /// @brief 逐帧更新，推进播放时间
    void update(float deltaTime); // deltaTime 单位：秒

    /// @brief 绘制
    void draw(IPainter& painter, const Rect& dstRect);

    /// @brief 播放控制
    void play() { _playing = true; }
    void stop() { _playing = false; }
    void reset();
    bool isPlaying() const { return _playing; }

public:
    /// @brief 帧切换信号，参数为当前帧索引
    signals::Signal<int> on_frame;

    /// @brief 帧事件信号，参数为事件名
    signals::Signal<std::string> on_frame_event;

private:
    std::vector<Frame> _frames;
    int _loop = 0;          // 循环次数
    float _rate = 1.0f;     // 播放速率
    bool _playing = false;
    float _time = 0.0f;     // 累计时间（秒）
    int _currentFrame = 0;  // 当前帧索引
    int _loopCount = 0;     // 已循环次数
};


/// @brief 动画管理器，负责从 XML 加载和管理动画模板
class AnimationManager final : public IResManager
{
public:
    AnimationManager() = default;
    ~AnimationManager() = default;

    /// @brief 加载目录下所有 XML 动画文件
    bool loadAll(const fs::path& dir, TextureManager& texMgr);

    /// @brief 加载单个 XML 动画文件
    bool loadFile(const fs::path& path, TextureManager& texMgr);

    /// @brief 获取指定名称的动画模板
    Animation* get(const std::string& name) const;

    /// @brief 获取所有动画名称
    std::vector<std::string> getAllNames() const;

    /// @brief 清除所有动画
    void clear() { _animations.clear(); }

private:
    std::unordered_map<std::string, std::unique_ptr<Animation>> _animations;
};


} // namespace engine
