#include "animation.h"
#include "texture.h"
#include <tinyxml2/tinyxml2.h>


namespace engine
{

// ============================================================
// Animation
// ============================================================

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::addFrame(int index, const Frame& frame)
{
    if (index < 0 || index > (int)_frames.size())
    {
        _frames.push_back(frame);
    }
    else
    {
        _frames.insert(_frames.begin() + index, frame);
    }
}

const Animation::Frame& Animation::getFrame(int index) const
{
    static Frame s_emptyFrame;
    if (index < 0 || index >= (int)_frames.size())
    {
        return s_emptyFrame;
    }
    return _frames[index];
}

TexTile* Animation::getCurrentTexture() const
{
    if (_frames.empty())
    {
        return nullptr;
    }
    return _frames[_currentFrame].texture;
}

void Animation::draw(IPainter& painter, const Rect& dstRect)
{
    painter.drawTexTile(getCurrentTexture(), dstRect);
}

void Animation::update(float deltaTime)
{
    if (!_playing || _frames.empty())
    {
        return;
    }

    _time += deltaTime * _rate;

    float frameDuration = _frames[_currentFrame].duration / 1000.0f;
    while (_time >= frameDuration && frameDuration > 0.0f)
    {
        _time -= frameDuration;

        // 切换到下一帧
        int nextFrame = _currentFrame + 1;
        if (nextFrame >= (int)_frames.size())
        {
            // 到达末帧
            if (!_loop)
            {
                // 不循环，停在最后一帧
                _playing = false;
                _currentFrame = (int)_frames.size() - 1;
                return;
            }

            // 循环播放
            nextFrame = 0;
        }

        _currentFrame = nextFrame;

        // 触发帧信号
        on_frame.emit(_currentFrame);

        // 如果有帧事件，触发事件信号
        const auto& evt = _frames[_currentFrame].event;
        if (!evt.empty())
        {
            on_frame_event.emit(evt);
        }

        frameDuration = _frames[_currentFrame].duration / 1000.0f;
    }
}

void Animation::reset()
{
    _time = 0.0f;
    _currentFrame = 0;
    _playing = false;
}


// ============================================================
// AnimationManager
// ============================================================

bool AnimationManager::loadAll(const fs::path& dir, TextureManager& texMgr)
{
    if (!fs::exists(dir))
    {
        LogError("AnimationManager: directory ({}) NOT exist.", dir.string());
        return false;
    }

    bool allOk = true;
    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (entry.path().extension() == ".xml")
        {
            bool ok = loadFile(entry.path(), texMgr);
            if (!ok)
            {
                LogWarn("AnimationManager: load file ({}) failed.", entry.path().string());
                allOk = false;
            }
        }
    }

    return allOk;
}

bool AnimationManager::loadFile(const fs::path& path, TextureManager& texMgr)
{
    using namespace tinyxml2;

    if (!fs::exists(path))
    {
        LogError("AnimationManager: file ({}) NOT exist.", path.string());
        return false;
    }

    XMLDocument xmlDoc;
    XMLError error = xmlDoc.LoadFile(path.string().c_str());
    if (error != XML_SUCCESS)
    {
        LogError("AnimationManager: load xml ({}) failed.", path.string());
        return false;
    }

    auto root = xmlDoc.RootElement();
    if (!root)
    {
        LogError("AnimationManager: no root element in ({}).", path.string());
        return false;
    }

    // 遍历所有 <anim> 元素
    auto animNode = root->FirstChildElement("anim");
    while (animNode)
    {
        const char* animName = animNode->Attribute("name");
        if (!animName || strlen(animName) == 0)
        {
            animNode = animNode->NextSiblingElement("anim");
            continue;
        }

        auto anim = std::make_unique<Animation>();

        // 遍历所有 <frame> 元素
        auto frameNode = animNode->FirstChildElement("frame");
        while (frameNode)
        {
            Animation::Frame frame;

            // 解析材质
            const char* texAttr = frameNode->Attribute("tex");
            if (texAttr && strlen(texAttr) > 0)
            {
                frame.texture = texMgr.getCfgTexTile(texAttr);
            }

            // 解析持续时间（毫秒）
            frame.duration = frameNode->IntAttribute("dur", 0);

            // 解析帧事件（可选）
            const char* evtAttr = frameNode->Attribute("evt");
            if (evtAttr)
            {
                frame.event = evtAttr;
            }

            anim->addFrame(-1, frame);

            frameNode = frameNode->NextSiblingElement("frame");
        }

        std::string name = animName;
        _animations[name] = std::move(anim);

        LogInfo("AnimationManager: loaded anim ({}), frames=({}).",
                name, _animations[name]->frameCount());

        animNode = animNode->NextSiblingElement("anim");
    }

    return true;
}

Animation* AnimationManager::get(const std::string& name) const
{
    auto it = _animations.find(name);
    if (it != _animations.end())
    {
        return it->second.get();
    }
    return nullptr;
}

std::vector<std::string> AnimationManager::getAllNames() const
{
    std::vector<std::string> names;
    names.reserve(_animations.size());
    for (const auto& [name, _] : _animations)
    {
        names.push_back(name);
    }
    return names;
}


} // namespace engine
