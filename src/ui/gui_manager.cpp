#include "gui_manager.h"
#include "widget.h"
#include "base_widgets.h"
#include "group_widgets.h"

namespace ui 
{
    int GuiManager::_widgetId = 0;

    Form* GuiManager::loadForm(const std::string& name, const fs::path& filepath)
    {
        auto form = createForm<Form>(name);
        if (!form)
        {
            LogError("create form : {} failed", name);
            return nullptr;
        }

        if (!form->load(filepath))
        {
            LogError("load form : {}, from file {} failed", name, filepath.string());
            return nullptr;
        }

        return form;
    }

    void GuiManager::init(engine::Application* app)
    {
        _app = app;

        _app->eventDispatcher().onKeyDown.connect(this, &GuiManager::onKeyDown, -1);
        _app->eventDispatcher().onKeyUp.connect(this, &GuiManager::onKeyUp, -1);

        _app->eventDispatcher().onMouseLeftClicked.connect(this, &GuiManager::onMouseLeftClick, -1);
        _app->eventDispatcher().onMouseRightClicked.connect(this, &GuiManager::onMouseRightClick, -1);

        _app->eventDispatcher().onMouseLeftDown.connect(this, &GuiManager::onMouseLeftDown, -1);
        _app->eventDispatcher().onMouseLeftUp.connect(this, &GuiManager::onMouseLeftUp, -1);

        _app->eventDispatcher().onMouseRightDown.connect(this, &GuiManager::onMouseRightDown, -1);
        _app->eventDispatcher().onMouseRightUp.connect(this, &GuiManager::onMouseRightUp, -1);

        _app->eventDispatcher().onMouseLeftDrag.connect(this, &GuiManager::onMouseLeftDrag, -1);
        _app->eventDispatcher().onMouseWheel.connect(this, &GuiManager::onMouseWheel, -1);
        _app->eventDispatcher().onMouseMotion.connect(this, &GuiManager::onMouseMotion, -1);

        _app->eventDispatcher().onWindowResized.connect(this, &GuiManager::onWindowResized, -1);
    }

    void GuiManager::update(float delta)
    {
        closePendingForms();

        for(auto& form : _forms)
        {
            form->update(delta);
        }

        if (_draggingData && _draggingData->widget) 
        {
            auto pos = _app->eventDispatcher().mousePos() + _draggingData->_offset;
            _draggingData->widget->setPos(pos);
        }
    }

    void GuiManager::draw()
    {
        for(auto& form : _forms)
        {
            form->draw();
        }

        if (_draggingData && _draggingData->widget)
        {
            _draggingData->widget->draw();
        }
    }

    void GuiManager::closePendingForms()
    {
        for (auto it = _forms.begin(); it != _forms.end(); )
        {
            auto ptr = *it;
            if (ptr)
            {
                auto iter = _pendingNames.find(ptr->name());
                if (iter != _pendingNames.end())
                {
                    ptr->onClose();
                    it = _forms.erase(it);
                    continue;
                }
            }

            it++;
        }

        _pendingNames.clear();
    }
        
    void GuiManager::closeForm(const std::string& name)
    {
        _pendingNames.insert(name);
    }

    void GuiManager::closeAllForms()
    {
        for (auto it = _forms.begin(); it != _forms.end(); it++)
        {
            auto ptr = *it;
            if (ptr)
            {
                _pendingNames.insert(ptr->name());
            }
        }
    }

    Form* GuiManager::getFormAtPos(const Vec2& pos)
    {
        for(auto it=_forms.rbegin(); it!=_forms.rend(); it++)
        {
            auto ptr = *it;
            if(ptr && ptr->visible())
            {
                Rect rect = {ptr->pos(), ptr->size()};
                if(rect.contains(pos))
                {
                    return ptr.get();
                }
            }
        }

        return nullptr;
    }

    Widget* GuiManager::getWidgetAtPos(const Vec2& pos, bool must_accept_event)
    {
        auto form = getFormAtPos(pos);
        if(!form)
        {
            return nullptr;
        }

        return form->getWidgetAtPos(pos, must_accept_event);
    }

    void GuiManager::moveToTop(const std::string& formName)
    {
        Form::SharedPtr form = nullptr;
        for(auto it=_forms.begin(); it!=_forms.end(); it++)
        {
            if((*it)->name() == formName)
            {
                form = *it;
                _forms.erase(it);
                break;
            }
        }

        _forms.push_back(form);
    }

    void GuiManager::onWindowResized(const Vec2& size)
    {
        for(auto& form : _forms)
        {
            form->onWindowResized(size);
        }
    }

    void GuiManager::onKeyDown(KeyCode key)
    {
    }

    void GuiManager::onKeyUp(KeyCode key)
    {
    }

    void GuiManager::onMouseLeftClick(const Vec2& pos)
    {
        auto form = getFormAtPos(pos);
        if(form && form->visible())
        {
            //moveToTop(form->name());

            form->onMouseLeftClick(pos);

            checkEventBreak(form);
        }
    }

    void GuiManager::onMouseRightClick(const Vec2& pos)
    {
        
        if (_draggingData)
        {
            //int index = card->getData<int>("index");
            //cardGroup->addWidget(ptr->widget, index);
            drop();
        }
        auto form = getFormAtPos(pos);
        if(form && form->visible())
        {
            //moveToTop(form->name());
            form->onMouseRightClick(pos);

            checkEventBreak(form);
        }
    }

    void GuiManager::onMouseLeftDown(const Vec2& pos)
    {
        auto form = getFormAtPos(pos);
        if (form && form->visible())
        {
            _mouseLeftCaptureForm = form;

            //moveToTop(form->name());
            form->onMouseLeftDown(pos);

            checkEventBreak(form);
        }
    }

    void GuiManager::onMouseLeftUp(const Vec2& pos)
    {
        auto form = _mouseLeftCaptureForm ? _mouseLeftCaptureForm : getFormAtPos(pos);
        if(form && form->visible())
        {
            form->onMouseLeftUp(pos);

            checkEventBreak(form);
        }

        _mouseLeftCaptureForm = nullptr;

        if (_draggingData)
        {
            drop();
        }
    }

    void GuiManager::onMouseRightDown(const Vec2& pos)
    {
        auto form = getFormAtPos(pos);
        if(form && form->visible())
        {
            form->onMouseRightDown(pos);

            checkEventBreak(form);
        }
    }

    void GuiManager::onMouseRightUp(const Vec2& pos)
    {
        auto form = getFormAtPos(pos);
        if(form && form->visible())
        {
            form->onMouseRightUp(pos);

            checkEventBreak(form);
        }
    }

    void GuiManager::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
    {
        if (_draggingData)
        {
            slotContext().setBreak(true);
            return;
        }

        auto form = _mouseLeftCaptureForm ? _mouseLeftCaptureForm : getFormAtPos(pos);
        if (!form || !form->visible()) 
        {
            return;
        }

        auto widget = form->hoverWidget();
        if (widget && widget->canDragOut())
        {
            drag(widget);

            slotContext().setBreak(true);
            return;
        }

        form->onMouseLeftDrag(pos, offset);

        checkEventBreak(form);
    }

    void GuiManager::onMouseWheel(const Vec2& pos, float dir)
    {
        auto form = getFormAtPos(pos);
        if(form && form->visible())
        {
            form->onMouseWheel(pos, dir);

            checkEventBreak(form);
        }
    }

    void GuiManager::onMouseMotion(const Vec2& pos, const Vec2& offset)
    {
        auto form = getFormAtPos(pos);
        if(form && form->visible())
        {
            form->onMouseMotion(pos, offset);
                
            checkEventBreak(form);
        }
    }

    void GuiManager::checkEventBreak(Form* form)
    {
        assert(form);

        auto widget = form->hoverWidget();
        if (widget && widget->acceptEvent())
        {
            slotContext().setBreak(true);
        }
    }

    void GuiManager::drag(Widget* widget)
    {
        if (!widget)
        {
            return;
        }

        auto parentGroup = dynamic_cast<Group*>(widget->parent());
        if (!parentGroup)
        {
            return;
        }

        auto wpos = widget->getAbsPos();
        auto mpos = _app->eventDispatcher().mousePos();

        _draggingData = std::make_shared<DraggingData>();
        _draggingData->widget= parentGroup->moveOut(widget);
        _draggingData->src_group = parentGroup;
        _draggingData->_offset= wpos - mpos;
    }

    void GuiManager::drop()
    {
        if (!_draggingData || !_draggingData->widget) 
        {
            return;
        }

        auto pos = _app->eventDispatcher().mousePos();
        auto widget = getWidgetAtPos(pos, false);
        
        while (widget) 
        {
            if (widget->canDropIn()) 
            {
                break;
            }
            widget = widget->parent();
        }

        if (!widget) 
        {
            auto src_group = dynamic_cast<Group*>(_draggingData->src_group);
            if (src_group)
            {
                int index = _draggingData->widget->getData<int>("index");
                src_group->addWidget(_draggingData->widget, index);

                _draggingData = nullptr;
            }
            return;
        }

        _draggingData->dst_group = widget;
        _draggingData->drop_screen_pos = pos;

        on_drop.emit(_draggingData);

        _draggingData = nullptr;
    }

    void GuiManager::emitCustomEvent(int eventId, const utility::VarList& varlist)
    {
        on_custom_event.emit(eventId, varlist);
    }

    Widget* GuiManager::createWidget(const std::string& type, Widget* parent)
    {
        auto it = _creators.find(type);
        if (it == _creators.end()) return nullptr;

        auto creator = it->second;
        if (!creator) { 
            return nullptr; 
        }

        auto name = std::format("{}-{}", type, generateId());
        auto widget = creator->create(name, parent);
        if (!widget) { 
            return nullptr; 
        }

        auto group = dynamic_cast<Group*>(parent);
        if (group) {
            group->addWidget(widget);
        }

        return widget.get();
    }

    bool GuiManager::loadTextureSet(const std::string& xmlcfg)
    {
        auto texset = _app->resourceManager().textureManager().loadTexSet(xmlcfg);
        return texset != nullptr;
    }

    TexTile* GuiManager::getTexTile(const std::string& texTile, const std::string& texset)
    {
        return _app->resourceManager().textureManager().getTexTile(texTile, texset);
    }

    TexTile* GuiManager::getCfgTexTile(const std::string& cfgTexTile)
    {
        return _app->resourceManager().textureManager().getCfgTexTile(cfgTexTile);
    }
}