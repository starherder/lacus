#pragma once

#include "engine/Application.h"
#include "utility/i_singleton.h"
#include "utility/dynamic_var.h"

#include "form.h"

namespace ui {

class GuiManager : public utility::ISingleton<GuiManager>,
                    public signals::SlotHandler
{
public:
    struct DraggingData 
    {
        WidgetPtr widget = nullptr;
        Widget* src_group = nullptr;
        Widget* dst_group = nullptr;

        Vec2 drop_screen_pos;
        Vec2 _offset;
    };

    using DraggingPtr = std::shared_ptr<DraggingData>;

public:
    signals::Signal<DraggingPtr> on_drop;

    signals::Signal<int, const utility::VarList&> on_custom_event;

public:
    void init(engine::Application* app);
    void update(float delta);
    void draw();

    auto windowSize() { return _app->window().getSize(); }
    auto windowPos() { return _app->window().getPosition(); }

    auto& renderer() { return _app->renderer(); }
    auto& painter() { return _app->painter(); }
    auto& resourceManager() { return _app->resourceManager(); }
    auto& audioPlayer() { return _app->audioPlayer(); }
    auto& fontManager() { return _app->resourceManager().fontManager(); }
    auto& textureManager() { return _app->resourceManager().textureManager(); }
    auto& eventDispatcher() { return _app->eventDispatcher(); }
    auto& frameTicker() { return _app->frameTicker(); }

    bool loadTextureSet(const std::string& xmlcfg);
    TexTile* getTexTile(const std::string& texTile, const std::string& texset="");

    template<typename FormType, typename... Args>
    FormType* createForm(const std::string& name, Args&... args);
    
    Form* loadForm(const std::string& name, const fs::path& filepath);

    template<typename FormType>
    FormType* getForm(const std::string& name);

    void closeForm(const std::string& name);
    void closeAllForms();

    void emitCustomEvent(int eventId, const utility::VarList& varlist);

    template<typename CreatorType>
    void addWidgetCreator();

    Widget* createWidget(const std::string& type, Widget* parent);

    int generateId() { return _widgetId++; }

    void moveToTop(const std::string& formName);

private:
    void onWindowResized(const Vec2& size);

    void onKeyDown(KeyCode key);
    void onKeyUp(KeyCode key);

    void onMouseLeftClick(const Vec2& pos);
    void onMouseRightClick(const Vec2& pos);
     
    void onMouseLeftDown(const Vec2& pos);
    void onMouseLeftUp(const Vec2& pos);
     
    void onMouseRightDown(const Vec2& pos);
    void onMouseRightUp(const Vec2& pos);
     
    void onMouseLeftDrag(const Vec2& pos, const Vec2& offset);
    void onMouseWheel(const Vec2& pos, float dir);
    void onMouseMotion(const Vec2& pos, const Vec2& offset);

    Form* getFormAtPos(const Vec2& pos);
    Widget* getWidgetAtPos(const Vec2& pos, bool must_accept_event=true);

    void drag(Widget* widget);
    void drop();

    void closePendingForms();

    void checkEventBreak(Form* form);

private:
    engine::Application* _app;

    Widget* _hoverWidget = nullptr;

    DraggingPtr _draggingData = nullptr;

    std::list<FormPtr> _forms;

    std::set<std::string> _pendingNames;

    std::map<std::string, WidgetCreatorPtr> _creators;

    static int _widgetId;
};


// ---------------------------------------------------------------------------

template<typename FormType, typename... Args>
FormType* GuiManager::createForm(const std::string& name, Args&... args)
{
    _pendingNames.erase(name);

    auto pform = getForm<FormType>(name);
    if(pform)
    {
        pform->show();
        return pform;
    }

    auto form = std::make_shared<FormType>(name, args...);
    form->show();

    _forms.push_back(form);
    return form.get();
}
    
template<typename FormType>
FormType* GuiManager::getForm(const std::string& name)
{
    for(auto& ptr : _forms)
    {
        if(ptr->name() == name)
        {
            return dynamic_cast<FormType*>(ptr.get());
        }
    }

    return nullptr;
}

template<typename CreatorType>
void GuiManager::addWidgetCreator()
{
    auto creator = std::make_shared<CreatorType>();
    _creators[creator->typeName()] = creator;
}
}