#include "scene.h"

namespace engine {


void Scene::update()
{
    onUpdate(_app.fpsChecker().deltaSeconds());
}

void Scene::draw()
{
    onDraw();
}

} // namespace engine