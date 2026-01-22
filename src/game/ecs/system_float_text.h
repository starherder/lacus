#pragma once


#include "system_utils.h"


namespace game {

class FloatTextSystem : public EcsSystem
{
public:
    FloatTextSystem(GameContext& context);
    ~FloatTextSystem();

    void update(float delta) override;

private:
    void onFloatTextStart(const EvtShowFloatText& e);

    std::string getFloatText(const EvtShowFloatText& e);
    Color getFloatColor(const EvtShowFloatText& e);
    Font* getFloatFont(const EvtShowFloatText& e);
};

    
}

