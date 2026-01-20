#include "scene.h"

namespace engine {


void Scene::update()
{
    if (!_ready) return;

    onUpdate(_app.frameTicker().deltaSeconds());
}

void Scene::draw()
{
    if (!_ready) return;

    onDraw();
}

} // namespace engine