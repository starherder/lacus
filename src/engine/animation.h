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

    /// @brief 设置是否循环播放
    void setLoop(bool loop) { _loop = loop; }
    bool loop() const { return _loop; }

    /// @brief 设置播放速率
    void setRate(float rate) { _rate = rate; }
    float rate() const { return _rate; }

    /// @brief 获取当前帧索引
    int getCurrentFrameIndex() const { return _currentFrame; }

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
    bool _loop = false;     // 是否循环
    float _rate = 1.0f;     // 播放速率
    bool _playing = false;
    float _time = 0.0f;     // 累计时间（秒）
    int _currentFrame = 0;  // 当前帧索引
};


/// @brief 动画帧配置（加载时解析，不含运行时指针）
struct FrameConfig
{
    std::string tex;        // 材质引用，如 "role:0-0"
    int duration = 0;       // 持续时长（毫秒）
    std::string event;      // 帧事件（可选）
};

/// @brief 动画配置（从XML解析的模板数据）
struct AnimationConfig
{
    std::string name;
    std::vector<FrameConfig> frames;
};


/// @brief 动画管理器，负责从 XML 加载动画配置并管理动画实例
class AnimationManager final : public IResManager
{
public:
    AnimationManager() = default;
    ~AnimationManager() = default;

    /// @brief 初始化，绑定纹理管理器引用（在 Application 初始化时调用）
    bool init(TextureManager& texMgr);

    /// @brief 加载目录下所有 XML 动画配置文件（仅解析配置）
    bool loadAll(const fs::path& dir);

    /// @brief 加载单个 XML 动画配置文件（仅解析配置）
    bool loadFile(const fs::path& path);

    /// @brief 根据配置创建动画实例
    /// @param cfgName 配置名（XML 中的 name 属性）
    /// @param instName 实例名，用于后续 get() 查找
    Animation* create(const std::string& cfgName, const std::string& instName);

    /// @brief 获取已创建的动画实例
    Animation* get(const std::string& name) const;

    /// @brief 获取所有动画配置名称
    std::vector<std::string> getAllNames() const;

    /// @brief 更新所有已创建的动画
    void Update(float deltaTime) override;

    /// @brief 清除所有配置和动画
    void clear() { _configs.clear(); _animations.clear(); }

private:
    std::unordered_map<std::string, AnimationConfig> _configs;
    std::unordered_map<std::string, std::unique_ptr<Animation>> _animations;
    TextureManager* _textureMgr = nullptr;
};


} // namespace engine
