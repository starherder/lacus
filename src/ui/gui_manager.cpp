#include "gui_manager.h"
#include "widget.h"
#include "base_widgets.h"
#include "group_widgets.h"

namespace ui {

        
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
        }

        void GuiManager::update(float delta)
        {
            for(auto& form : _forms)
            {
                form->update(delta);
            }

            if (_draggingData && _draggingData->widget) 
            {
                auto pos = _app->eventDispatcher().mousePos() + _draggingData->offset;
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
        
        void GuiManager::closeForm(const std::string& name)
        {
            for(auto it=_forms.begin(); it!=_forms.end(); it++)
            {
                auto ptr = *it;
                ptr->onClose();

                _forms.erase(it);
                return;
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

        Widget* GuiManager::getWidgetAtPos(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if(!form)
            {
                return nullptr;
            }

            return form->getWidgetAtPos(pos);
        }

        void GuiManager::moveFormTop(const std::string& formName)
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
                moveFormTop(form->name());
                form->onMouseLeftClick(pos);

                slot_context().setBreak(true);
            }
        }

        void GuiManager::onMouseRightClick(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                moveFormTop(form->name());
                form->onMouseRightClick(pos);

                slot_context().setBreak(true);
            }
        }

        void GuiManager::onMouseLeftDown(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if (form && form->visible())
            {
                moveFormTop(form->name());
                form->onMouseLeftDown(pos);

                slot_context().setBreak(true);
            }
        }

        void GuiManager::onMouseLeftUp(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseLeftUp(pos);

                slot_context().setBreak(true);
            }

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

                slot_context().setBreak(true);
            }
        }

        void GuiManager::onMouseRightUp(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseRightUp(pos);

                slot_context().setBreak(true);
            }
        }

        void GuiManager::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
        {
            if (_draggingData)
            {
                return;
            }

            auto form = getFormAtPos(pos);
            if (!form || !form->visible() || !form->focused()) 
            {
                return;
            }

            auto widget = form->hoverWidget();
            if (widget && widget->canDragOut())
            {
                drag(widget);
                return;
            }

            form->onMouseLeftDrag(pos, offset);
            slot_context().setBreak(true);
        }

        void GuiManager::onMouseWheel(const Vec2& pos, float dir)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseWheel(pos, dir);

                slot_context().setBreak(true);
            }
        }

        void GuiManager::onMouseMotion(const Vec2& pos, const Vec2& offset)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseMotion(pos, offset);

                slot_context().setBreak(true);
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
            _draggingData->source = parentGroup;
            _draggingData->offset= wpos - mpos;

            on_drag_start.emit(widget);
        }

        GuiManager::DraggingPtr GuiManager::fetchDraggingData()
        {
            if (_draggingData) 
            {
                DraggingPtr ptr = _draggingData;
                _draggingData = nullptr;

                return ptr;
            }

            return nullptr;
        }

        void GuiManager::drop()
        {
            auto pos = _app->eventDispatcher().mousePos();
            auto widget = getWidgetAtPos(pos);
            
            on_drop.emit(widget, pos);
        }
}