#include <SDL3/SDL.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>  // 包含 π 常量
#include <glm/gtx/vector_angle.hpp>  // 角度计算扩展

#include "utility/string_util.h"

#define PI		3.1415926535898f
#define PI_2	6.2831853071795f
#define PI_HALF		1.57079632f


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

    Vec2 pos() const;
    Vec2 size() const;

    bool contains(const Vec2& point) const;
    bool contains(const Rect& rect) const;
    
    Rect intersect(const Rect& other) const;
    bool isIntersect(const Rect& other) const;

    operator SDL_FRect() const;
    operator SDL_Rect() const;

    bool operator==(const Rect& other) const;
    bool operator!=(const Rect& other) const;
};

//------------------------------------------------------------

inline bool operator < (const Vec2i& lval, const Vec2i& rval) {
    if(lval.x == rval.x) return lval.y < rval.y;
    return lval.x < rval.x;
}

inline Vec2 SafeNormal(const Vec2 val)
{
    if(val.x==0.0f && val.y==0.0f) return Vec2{0.0f, 0.0f};
    return glm::normalize(val);
}

inline SDL_FPoint ToPoint(const Vec2& v) {
    return SDL_FPoint{ v.x, v.y };
}

inline Vec2 ToVec2(const SDL_FPoint& v) {
    return Vec2{ v.x, v.y };
}

inline Vec2 ToVec2(const std::string& s) {
    try {
        auto arr = utility::StringUtil::split(s, ',');
        if (arr.size() != 2) { return Vec2{0, 0}; }

        return { std::stof(arr[0].data()),
                std::stof(arr[1].data()) };
    }
    catch (const std::exception& ) {
        return { 0.0f, 0.0f};
    }
}

inline Rect ToRect(const std::string& s) {
   try {
        auto arr = utility::StringUtil::split(s, ',');
        assert(arr.size() == 4);

        return { std::stof(arr[0].data()), 
                std::stof(arr[1].data()), 
                std::stof(arr[2].data()), 
                std::stof(arr[3].data()) };
   }
   catch (const std::exception& ) {
        return { 0.0f, 0.0f, 0.0f, 0.0f };
   }
}


class Geometry
{
public:
    // 随便的排序函数，顺序不重要
    template<typename VecType>
    struct Vec2Comparator {
        bool operator()(const VecType& lval, const VecType& rval) const {
            if (lval.x == rval.x) {
                return lval.y < rval.y;
            }
            else {
                return lval.x < rval.x;
            }
        }
    };

    using Vec2fComparator = Vec2Comparator<Vec2>;
    using Vec2iComparator = Vec2Comparator<Vec2i>;

public:
    static std::vector<Vec2i> bresenhamLine(const Vec2i src, const Vec2i& dst);

    // p 绕着 c 点旋转 r 弧度
    static Vec2 RotatePoint(const Vec2& p, const Vec2 c, float r);
};

}