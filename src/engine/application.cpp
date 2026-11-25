#include "application.h"
#include "resource.h"
#include <chrono>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>

namespace engine {

Application::Application() 
{
    _renderer = std::make_unique<Renderer>();
    _window = std::make_unique<Window>();
    _resourceMgr = std::make_unique<ResourceManager>(*this);
    _audioPlayer = std::make_unique<AudioPlayer>();
    
#ifdef USE_IMGUI_AS_RENDER_ENGINE
    _ipainter = std::make_unique<ImPainter>(*this);
#else
    _ipainter = std::make_unique<Painter>(*this);
    //_ipainter = std::make_unique<GfxPainter>(*this);
#endif
}

Application::~Application() 
{
    close();
}

fs::path Application::runPath() 
{ 
    return PathUtils::get_executable_path(); 
}

const fs::path& Application::resPath() 
{ 
    return _resPath; 
}

Plugin* Application::getPlugin(const std::string& name)
{
    for(auto& [prio, plugin] : _plugins)
    {
        if(plugin && plugin->name() == name)
        {
            return plugin.get();
        }
    }

    return nullptr;
}

bool Application::removePlugin(const std::string& name) 
{
    for(auto it=_plugins.begin(); it!= _plugins.end(); it++)
    {
        auto& plugin = it->second;
        if(plugin && plugin->name() == name)
        {
            plugin->uninstall();
            _plugins.erase(it);
            return true;
        }
    }
    return false;
}

void Application::run() 
{
    while (_running)
    {
        input();

        if (preFrame())
        {
            update();

            draw();

            drawUI();

            postFrame();
        }
    }

    close();
}

bool Application::init(std::string_view configFile)
{
    SDL_Log("---------------- engine init ----------------");

    if(!initConfig(configFile)) 
    {
        SDL_Log("init config failed.");
        return false;
    }

#ifdef _WIN32
    PlatformUtils::setConsoleVisible(_config.console.visible);
#endif

    if(!initLog()) 
    {
        SDL_Log("init spdlog failed.");
        return false;
    }

    if(!initWindow()) 
    {
        return false;
    }
    
    if(!initRenderer()) 
    {
        return false;
    }

    if(!initAudioPlayer())
    {
        return false;
    }

    if(!initAnimation())
    {
        return false;
    }

    _frameTicker.init(_config.window.fps);

    LogInfo("---------------- engine init OK ----------------");
    return true;
}

bool Application::initConfig(std::string_view config_file) 
{
    fs::path path;
    fs::path basePath;
    if (!PlatformUtils::findConfigPath(config_file, path, basePath))
    {
        LogError("config file({}) NOT found.", config_file);
        return false;
    }

    if(!_config.load(path, basePath))
    {
        return false;
    }

    _resPath = _config.res.path;

    _resourceMgr->setResPath(_resPath);
    return true;
}

bool Application::initLog() 
{
    try
    {
        fs::path logPath = _config.log.file;
        if (logPath.is_relative())
        {
            logPath = fs::current_path() / logPath;
        }

        if (logPath.has_parent_path())
        {
            fs::create_directories(logPath.parent_path());
        }

        spdlog::drop("lacus");
        auto logger = spdlog::basic_logger_mt("lacus", logPath.string(), true);
        spdlog::set_default_logger(logger);
        spdlog::set_level((spdlog::level::level_enum)_config.log.level);
        spdlog::set_pattern(_config.log.pattern);
        spdlog::flush_every(std::chrono::seconds(1));

        _logInited = true;
        LogInfo("log file: {}", logPath.string());
        return true;
    }
    catch (const spdlog::spdlog_ex& e)
    {
        SDL_Log("init spdlog failed: %s", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        SDL_Log("init spdlog failed: %s", e.what());
        return false;
    }
}

bool Application::initWindow()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        LogError("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    int numDrivers = _renderer->getNumRenderDrivers();
    if (numDrivers <= 0) {
        LogError("No render drivers available");
        return false;
    }

    for (int i = 0; i < numDrivers; ++i)
    {
        const char* name = _renderer->getRenderDriver(i);
        LogInfo("Render driver[{}]: {}", i, name);
    }

    auto windowFlags = (WindowFlags)((Uint64)WindowFlags::Resizable | (Uint64)WindowFlags::HighPixelDensity);
    bool res = _window->create(_config.window.title.c_str(), _config.window.width, _config.window.height, windowFlags);
    if (!res) {
        LogError("Failed to create window");
        return false;
    }

    _window->setPosition({SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED});

    _eventDispatcher = std::make_unique<EventDispatcher>();
    _eventDispatcher->onQuit.connect([&]{_running = false;});

    LogInfo("window created.");
    return true;
}

bool Application::initRenderer()
{
    auto res = _renderer->init(_window->getSdlWindow());
    if (!res) {
        LogError("Failed to create renderer");
        return false;
    }

    painter().init();
    _painterInited = true;

    LogInfo("renderer created.");
    return true;
}

bool Application::initAudioPlayer()
{
    auto& audioMgr = resourceManager().audioManager();
    auto res = _audioPlayer->init(&audioMgr);
    if(!res)
    {
        LogError("fail to create audio player");
        return false;
    }

    _audioPlayer->setMusicVolume(_config.sound.music_volumn);
    _audioPlayer->setSoundVolume(_config.sound.sound_volumn);

    //LogInfo("audio player created.");
    return true;
}

bool Application::initAnimation()
{
    return _resourceMgr->animationManager().init(_resourceMgr->textureManager());
}

void Application::update()
{
    // 先更新资源管理器（如动画）
    _resourceMgr->Update(_frameTicker.deltaSeconds());

    // 再更新插件
    for(auto& plugin : _plugins)
    {
        plugin.second->update();
    }
}

void Application::draw() 
{
    for(auto& plugin : _plugins)
    {
        plugin.second->draw();
    }   
}

void Application::drawUI() 
{
    for(auto& plugin : _plugins)
    {
        plugin.second->drawUI();
    }   
}

bool Application::close() 
{
    if (_closed)
    {
        return true;
    }
    _closed = true;

    for(auto& plugin : _plugins)
    {
        plugin.second->close();
    }   
    _plugins.clear();

    if (_logInited)
    {
        LogInfo("---------------- engine closed ----------------");
    }

    if (_audioPlayer)
    {
        _audioPlayer->close();
    }

    if (_ipainter)
    {
        if (_painterInited)
        {
            _ipainter->quit();
            _painterInited = false;
        }
        _ipainter.reset();
    }

    if (_renderer)
    {
        _renderer->destroyTextRenderer();
    }

    _resourceMgr.reset();
    _audioPlayer.reset();
    _eventDispatcher.reset();
    _renderer.reset();
    _window.reset();

    SDL_Quit();
    if (_logInited)
    {
        spdlog::shutdown();
        _logInited = false;
    }
    return true;
}

void Application::input() 
{
    _eventDispatcher->run();
}

bool Application::preFrame() 
{
    if (!_frameTicker.check())
    {
        return false;
    }

    _renderer->setClipRect({Vec2{0,0}, _window->getSize()});

    _renderer->setDrawColor(_renderer->getClearColor());

    _renderer->clear();

    painter().preFrame();

    _renderer->setDrawColor(_renderer->getClearColor());

    return true;
}

bool Application::postFrame() 
{
    painter().postFrame();

    _renderer->present();

    return true;
}



} // namespace engine
