#include "scene.h"

namespace engine {


void Scene::update()
{
    onUpdate(_app.frameTicker().deltaSeconds());
}

void Scene::draw()
{
    onDraw();
}

} // namespace engine