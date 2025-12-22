#pragma once

#include "engine/Application.h"
#include "utility/i_singleton.h"
#include "form.h"

namespace ui {
    
class GuiManager : public utility::ISingleton<GuiManager>,
                    public sigslot::SlotHandler
{
public:
    void init(engine::Application* app);

    void update(float delta);

    void draw();

    auto& renderer() { return _app->renderer(); }
    auto& resourceManager() { return _app->resourceManager(); }
    auto& audioPlayer() { return _app->audioPlayer(); }
    auto& fontManager() { return _app->resourceManager().fontManager(); }
    auto& textureManager() { return _app->resourceManager().textureManager(); }
    auto& eventDispatcher() { return _app->eventDispatcher(); }
    auto& fpsChecker() { return _app->fpsChecker(); }
        
    template<typename FormType, typename... Args>
    FormType* showForm(const std::string& name, const Args&... args);
        
    template<typename FormType>
    FormType* getForm(const std::string& name);

    void closeForm(const std::string& name);

private:
    void onKeyDown(int key);
    void onKeyUp(int key);

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

private:
    engine::Application* _app;

    Widget* _hoverWidget = nullptr;

    std::list<Form::SharedPtr> _forms;
};


template<typename FormType, typename... Args>
FormType* GuiManager::showForm(const std::string& name, const Args&... args)
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