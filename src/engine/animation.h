#pragma once

#include "engine/wrapper.h"
#include "engine/texture.h"
#include "engine/misc.h"
#include "i_painter.h"


namespace engine
{

class Animation final
{
public:
    struct Frame
    {
        TexTile* texture = nullptr;
        int duration = 0;
        std::string event;
    };

public:
    Animation();
    ~Animation();

    void addFrame(int index, const Frame& frame);

    int frameCount() const { return (int)_frames.size(); }

    const Frame& getFrame(int index) const;

    void setLoop(bool loop) { _loop = loop; }
    bool loop() const { return _loop; }

    void setRate(float rate) { _rate = rate; }
    float rate() const { return _rate; }

    int getCurrentFrameIndex() const { return _currentFrame; }

    TexTile* getCurrentTexture() const;

    void update(float deltaTime);

    void draw(IPainter& painter, const Rect& dstRect);

    void play() { _playing = true; }
    void stop() { _playing = false; }
    void reset();
    bool isPlaying() const { return _playing; }

public:
    signals::Signal<int> on_frame;

    signals::Signal<std::string> on_frame_event;

private:
    std::vector<Frame> _frames;
    bool _loop = false;
    float _rate = 1.0f;
    bool _playing = false;
    float _time = 0.0f;
    int _currentFrame = 0;
};


struct FrameConfig
{
    std::string tex;
    int duration = 0;
    std::string event;
};

struct AnimationConfig
{
    std::string name;
    std::vector<FrameConfig> frames;
};


class AnimationManager final : public IResManager
{
public:
    AnimationManager() = default;
    ~AnimationManager() = default;

    bool init(TextureManager& texMgr);

    bool loadAll(const fs::path& dir);

    bool loadFile(const fs::path& path);

    Animation* create(const std::string& cfgName, const std::string& instName);

    Animation* get(const std::string& name) const;

    bool remove(const std::string& name);

    std::vector<std::string> getAllNames() const;

    void Update(float deltaTime) override;

    void clear() { _configs.clear(); _animations.clear(); }

private:
    std::unordered_map<std::string, AnimationConfig> _configs;
    std::unordered_map<std::string, std::unique_ptr<Animation>> _animations;
    TextureManager* _textureMgr = nullptr;
};


} // namespace engine