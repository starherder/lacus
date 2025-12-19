#include "scene.h"

namespace engine {


void Scene::update()
{
    onUpdate(_app.fpsChecker().deltaSeconds());
}

void Scene::render()
{
    onRender();
}

void Scene::renderUI()
{
    onRenderUI();
}


} // namespace engine