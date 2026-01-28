#include "scene.h"

namespace engine {


void Scene::update()
{
    if (!ready()) return;

    onUpdate(_app.frameTicker().deltaSeconds());
}

void Scene::draw()
{
    if (!ready()) return;

    onDraw();
}

} // namespace engine