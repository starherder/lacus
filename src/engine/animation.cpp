#include "animation.h"
#include "texture.h"
#include <tinyxml2/tinyxml2.h>


namespace engine
{

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

        int nextFrame = _currentFrame + 1;
        if (nextFrame >= (int)_frames.size())
        {
            if (!_loop)
            {
                _playing = false;
                _currentFrame = (int)_frames.size() - 1;
                return;
            }

            nextFrame = 0;
        }

        _currentFrame = nextFrame;

        on_frame.emit(_currentFrame);

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


bool AnimationManager::loadAll(const fs::path& dir)
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
            bool ok = loadFile(entry.path());
            if (!ok)
            {
                LogWarn("AnimationManager: load file ({}) failed.", entry.path().string());
                allOk = false;
            }
        }
    }

    return allOk;
}

bool AnimationManager::loadFile(const fs::path& path)
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

    auto animNode = root->FirstChildElement("anim");
    while (animNode)
    {
        const char* animName = animNode->Attribute("name");
        if (!animName || strlen(animName) == 0)
        {
            animNode = animNode->NextSiblingElement("anim");
            continue;
        }

        std::string name = animName;
        AnimationConfig config;
        config.name = name;

        auto frameNode = animNode->FirstChildElement("frame");
        while (frameNode)
        {
            FrameConfig frame;

            const char* texAttr = frameNode->Attribute("tex");
            if (texAttr && strlen(texAttr) > 0)
            {
                frame.tex = texAttr;
            }

            frame.duration = frameNode->IntAttribute("dur", 0);

            const char* evtAttr = frameNode->Attribute("evt");
            if (evtAttr)
            {
                frame.event = evtAttr;
            }

            config.frames.push_back(frame);

            frameNode = frameNode->NextSiblingElement("frame");
        }

        _configs[name] = std::move(config);

        LogInfo("AnimationManager: loaded anim config ({}), frames=({}).",
                name, _configs[name].frames.size());

        animNode = animNode->NextSiblingElement("anim");
    }

    return true;
}

bool AnimationManager::init(TextureManager& texMgr)
{
    _textureMgr = &texMgr;
    return true;
}

Animation* AnimationManager::create(const std::string& cfgName, const std::string& instName)
{
    if (!_textureMgr)
    {
        LogError("AnimationManager: create failed, TextureManager not initialized.");
        return nullptr;
    }

    auto configIt = _configs.find(cfgName);
    if (configIt == _configs.end())
    {
        LogError("AnimationManager: create failed, config ({}) not found.", cfgName);
        return nullptr;
    }

    auto animIt = _animations.find(instName);
    if (animIt != _animations.end())
    {
        LogWarn("AnimationManager: instance ({}) already exists, return existing.", instName);
        return animIt->second.get();
    }

    auto anim = std::make_unique<Animation>();
    for (const auto& frameCfg : configIt->second.frames)
    {
        Animation::Frame frame;
        frame.texture = _textureMgr->getCfgTexTile(frameCfg.tex);
        frame.duration = frameCfg.duration;
        frame.event = frameCfg.event;
        anim->addFrame(-1, frame);
    }

    auto* result = anim.get();
    _animations[instName] = std::move(anim);

    LogInfo("AnimationManager: created anim inst ({}), config ({}), frames=({}).",
            instName, cfgName, result->frameCount());
    return result;
}

void AnimationManager::Update(float deltaTime)
{
    for (auto& [name, anim] : _animations)
    {
        anim->update(deltaTime);
    }
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

bool AnimationManager::remove(const std::string& name)
{
    return _animations.erase(name) > 0;
}

std::vector<std::string> AnimationManager::getAllNames() const
{
    std::vector<std::string> names;
    names.reserve(_configs.size());
    for (const auto& [name, _] : _configs)
    {
        names.push_back(name);
    }
    return names;
}


} // namespace engine