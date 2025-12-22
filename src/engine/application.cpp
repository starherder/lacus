#include "application.h"
#include "resource.h"
#include <memory>

namespace engine {

Application::Application() 
{
    _renderer = std::make_unique<Renderer>();
    _window = std::make_unique<Window>();
    _resourceMgr = std::make_unique<ResourceManager>(*this);
    _audioPlayer = std::make_unique<AudioPlayer>();
}

Application::~Application() 
{
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

bool Application::init(std::string_view configFile, std::string_view logFile)
{
    SDL_Log("---------------- engine init ----------------");

    if(!initConfig(configFile)) 
    {
        SDL_Log("init config failed.");
        return false;
    }

    if(!initLog(logFile)) 
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

    _fps_checker.init(_config.window.fps);

    spdlog::info("---------------- engine init OK ----------------");
    return true;
}

bool Application::initConfig(std::string_view config_file) 
{
    auto path = fs::current_path() / config_file; 
    if(!_config.load(path)) 
    {
        return false;
    }

    _resPath = _config.res.path;

    _resourceMgr->setResPath(_resPath);
    return true;
}

bool Application::initLog(std::string_view logFile) 
{
    spdlog::set_level((spdlog::level::level_enum)_config.log.level);
    spdlog::set_pattern(_config.log.pattern);
    
    return true;
}

bool Application::initWindow() 
{
    int numDrivers = _renderer->getNumRenderDrivers();
    if (numDrivers <= 0) {
        spdlog::error("No render drivers available");
        return false;
    }
    
    for (int i = 0; i < numDrivers; ++i) 
    {
        const char* name = _renderer->getRenderDriver(i);
        spdlog::info("Render driver[{}]: {}", i, name);
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        spdlog::error("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    bool res = _window->create(_config.window.title.c_str(), _config.window.width, _config.window.height, WindowFlags::Resizable);
    if (!res) {
        spdlog::error("Failed to create window");
        return false;
    }

    _window->setPosition({SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED});

    _eventDispatcher = std::make_unique<EventDispatcher>();
    _eventDispatcher->onQuit.connect([&]{_running = false;});

    spdlog::info("window created.");
    return true;
}

bool Application::initRenderer()
{
    auto res = _renderer->init(_window->getSdlWindow());
    if (!res) {
        spdlog::error("Failed to create renderer");
        return false;
    }

    spdlog::info("renderer created.");
    return true;
}

bool Application::initAudioPlayer()
{
    auto& audioMgr = resourceManager().audioManager();
    auto res = _audioPlayer->init(&audioMgr);
    if(!res)
    {
        spdlog::error("fail to create audio player");
        return false;
    }

    spdlog::info("audio player created.");
    return true;
}

void Application::update() 
{
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
    for(auto& plugin : _plugins)
    {
        plugin.second->close();
    }   
    
    spdlog::info("---------------- engine closed ----------------");

    SDL_Quit();
    return true;
}

void Application::input() 
{
    _eventDispatcher->run();
}

bool Application::preFrame() 
{
    if (!_fps_checker.check()) 
    {
        return false;
    }

    _renderer->setDrawColor(Color::LightBlue);

    _renderer->clear();
    
    return true;
}

bool Application::postFrame() 
{
    _renderer->present();

    return true;
}



} // namespace engine