#include <SDL3/SDL.h>
#include <glm/glm.hpp>


namespace engine {

using Vertex = SDL_Vertex;

using Vec2 = glm::fvec2;
using Vec2f = glm::fvec2;
using Vec3f = glm::fvec3;
using Vec4f = glm::fvec4;

using Vec2i = glm::ivec2;
using Vec3i = glm::ivec3;
using Vec4i = glm::ivec4;


class Rect
{
public:
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    Rect() = default;
    ~Rect() = default;

    Rect(const Vec2& pos, const Vec2& size);
    Rect(float x_, float y_, float w_, float h_);
    Rect(int x_, int y_, int w_, int h_);

    Rect(const SDL_FRect& frect);
    Rect(const SDL_Rect& rect);

    Vec2 pos();
    Vec2 size();
    bool contains(const Vec2& point) const;

    operator SDL_FRect() const;
    operator SDL_Rect() const;

    bool operator==(const Rect& other) const;
    bool operator!=(const Rect& other) const;
};



}