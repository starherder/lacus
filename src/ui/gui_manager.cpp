#include "gui_manager.h"

namespace ui {

        
        void GuiManager::init(engine::Application* app)
        {
             _app = app;

             _app->eventDispatcher().onKeyDown.connect(this, &GuiManager::onKeyDown);
             _app->eventDispatcher().onKeyUp.connect(this, &GuiManager::onKeyUp);

             _app->eventDispatcher().onMouseLeftClicked.connect(this, &GuiManager::onMouseLeftClick);
             _app->eventDispatcher().onMouseRightClicked.connect(this, &GuiManager::onMouseRightClick);

             _app->eventDispatcher().onMouseLeftDown.connect(this, &GuiManager::onMouseLeftDown);
             _app->eventDispatcher().onMouseLeftUp.connect(this, &GuiManager::onMouseLeftUp);

             _app->eventDispatcher().onMouseRightDown.connect(this, &GuiManager::onMouseRightDown);
             _app->eventDispatcher().onMouseRightUp.connect(this, &GuiManager::onMouseRightUp);

             _app->eventDispatcher().onMouseLeftDrag.connect(this, &GuiManager::onMouseLeftDrag);
             _app->eventDispatcher().onMouseWheel.connect(this, &GuiManager::onMouseWheel);
             _app->eventDispatcher().onMouseMotion.connect(this, &GuiManager::onMouseMotion);
        }

        void GuiManager::update(float delta)
        {
            for(auto& form : _forms)
            {
                form->update(delta);
            }
        }

        void GuiManager::draw()
        {
            for(auto& form : _forms)
            {
                form->draw();
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
                    FRect rect = {ptr->pos().x, ptr->pos().y, ptr->size().x, ptr->size().y};
                    if(pos.x >= rect.x && pos.x <= rect.x+rect.w && pos.y >= rect.y && pos.y <= rect.y+rect.h)
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

        void GuiManager::onKeyDown(int key)
        {
        }

        void GuiManager::onKeyUp(int key)
        {
        }

        void GuiManager::onMouseLeftClick(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                moveFormTop(form->name());
                form->onMouseLeftClick(pos);
            }
        }

        void GuiManager::onMouseRightClick(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                moveFormTop(form->name());
                form->onMouseRightClick(pos);
            }
        }

        void GuiManager::onMouseLeftDown(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if (form && form->visible())
            {
                moveFormTop(form->name());
                form->onMouseLeftDown(pos);
            }
        }

        void GuiManager::onMouseLeftUp(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseLeftUp(pos);
            }
        }

        void GuiManager::onMouseRightDown(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseRightDown(pos);
            }
        }

        void GuiManager::onMouseRightUp(const Vec2& pos)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseRightUp(pos);
            }
        }

        void GuiManager::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseLeftDrag(pos, offset);
            }
        }

        void GuiManager::onMouseWheel(const Vec2& pos, float dir)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseWheel(pos, dir);
            }
        }

        void GuiManager::onMouseMotion(const Vec2& pos, const Vec2& offset)
        {
            auto form = getFormAtPos(pos);
            if(form && form->visible())
            {
                form->onMouseMotion(pos, offset);
            }
        }
}