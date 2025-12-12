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

fs::path Application::resPath() 
{ 
    return _res_path; 
}

bool Application::hasPlugin(const std::string& name) 
{
    return _plugins.find(name) != _plugins.end();
}

bool Application::removePlugin(const std::string& name) 
{
    if (!hasPlugin(name)) {
        return false;
    }

    _plugins[name]->Uninstall();
    _plugins.erase(name);
    return true;
}

void Application::run() 
{
    bool res = init();
    if (!res)
    {
        return;
    }

    while (_running)
    {
        input();

        if (preFrame())
        {
            update();

            draw();

            postFrame();
        }
    }

    close();
}

bool Application::init() 
{
    SDL_Log("---------------- engine init ----------------");

    if(!initConfig()) 
    {
        SDL_Log("init config failed.");
        return false;
    }

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

    if(!initPlugins())
    {
        return false;
    }

    _frame_checker.init(_config.window.fps);

    spdlog::info("---------------- engine init OK ----------------");
    return true;
}

bool Application::initPlugins()
{
    for(auto& plugin : _plugins)
    {
        plugin.second->onInit();
    }

    return true;
}

bool Application::initConfig() 
{
    auto path = fs::current_path() / "res/system.json";
    if(!_config.load(path)) 
    {
        return false;
    }

    _res_path = _config.res.path;
    return true;
}

bool Application::initLog() 
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

    bool res = _window->createWindow(_config.window.title.c_str(), _config.window.width, _config.window.height, WindowFlags::Resizable);
    if (!res) {
        spdlog::error("Failed to create window");
        return false;
    }

    _window->setWindowPosition(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

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
        plugin.second->onUpdate();
    }   
}

void Application::draw() 
{
    for(auto& plugin : _plugins)
    {
        plugin.second->onDraw();
    }   
}

void Application::processEvent(Event& event) 
{
    for(auto& plugin : _plugins)
    {
        plugin.second->onEvent(event);
    }

    switch (event.type) 
    {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: 
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: 
        case SDL_EVENT_MOUSE_MOTION: 
        {
        }break;
        case SDL_EVENT_QUIT: 
        {
            _running = false;
        }break;
        default:{
            break;
        }
    }
}

bool Application::close() 
{
    for(auto& plugin : _plugins)
    {
        plugin.second->onClose();
    }   
    
    spdlog::info("---------------- engine closed ----------------");

    SDL_Quit();
    return true;
}

void Application::input() 
{
    Event event;
    while (SDL_PollEvent(&event)) {
        processEvent(event);
    }
}

bool Application::preFrame() 
{
    if (!_frame_checker.check()) 
    {
        return false;
    }

    _renderer->setDrawColor({0, 0, 0, 0});

    _renderer->clear();
    
    return true;
}

bool Application::postFrame() 
{
    _renderer->present();

    return true;
}



} // namespace engine