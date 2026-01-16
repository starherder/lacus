#pragma once

#include "engine/Application.h"
#include "utility/i_singleton.h"
#include "utility/dynamic_var.h"

#include "form.h"

namespace ui {

class Widget;
using WidgetPtr = std::shared_ptr<Widget>;



class GuiManager : public utility::ISingleton<GuiManager>,
                    public signals::SlotHandler
{
public:
    struct DraggingData 
    {
        WidgetPtr widget = nullptr;
        Widget* source = nullptr;
        Vec2 offset;
    };

    using DraggingPtr = std::shared_ptr<DraggingData>;

public:
    signals::Signal<Widget*> on_drag_start;
    signals::Signal<Widget*, const Vec2&> on_drop;

    signals::Signal<int, const utility::VarList&> on_custom_event;

public:
    void init(engine::Application* app);
    void update(float delta);
    void draw();

    auto windowSize() { return _app->window().getSize(); }
    auto windowPos() { return _app->window().getPosition(); }

    auto& renderer() { return _app->renderer(); }
    auto& painter() { return _app->painter(); }
    auto& gfxPainter() { return _app->gfx_painter(); }
    auto& imPainter() { return _app->im_painter(); }
    auto& resourceManager() { return _app->resourceManager(); }
    auto& audioPlayer() { return _app->audioPlayer(); }
    auto& fontManager() { return _app->resourceManager().fontManager(); }
    auto& textureManager() { return _app->resourceManager().textureManager(); }
    auto& eventDispatcher() { return _app->eventDispatcher(); }
    auto& frameTicker() { return _app->frameTicker(); }

    template<typename FormType, typename... Args>
    FormType* showForm(const std::string& name, Args&... args);
        
    template<typename FormType>
    FormType* getForm(const std::string& name);

    void closeForm(const std::string& name);

    DraggingPtr fetchDraggingData();

    void emitCustomEvent(int eventId, const utility::VarList& varlist);

private:
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
    Widget* getWidgetAtPos(const Vec2& pos);

    void moveFormTop(const std::string& formName);

    void drag(Widget* widget);
    void drop();

    void closePendingForms();

private:
    engine::Application* _app;

    Widget* _hoverWidget = nullptr;

    DraggingPtr _draggingData = nullptr;

    std::list<FormPtr> _forms;

    std::set<std::string> _pendingNames;
};


// ---------------------------------------------------------------------------

template<typename FormType, typename... Args>
FormType* GuiManager::showForm(const std::string& name, Args&... args)
{
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

}