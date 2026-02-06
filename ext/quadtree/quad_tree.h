#pragma once

#include <cassert>
#include <algorithm>
#include <array>
#include <memory>
#include <type_traits>
#include <vector>
#include <set>

namespace quadtree
{

template<typename T>
struct Vector2 {
    T x, y;
    constexpr Vector2<T>(T X = 0, T Y = 0) noexcept : x(X), y(Y){}
    constexpr Vector2<T>& operator += (const Vector2<T>& other) noexcept{ x += other.x; y += other.y; return *this; }
    constexpr Vector2<T>& operator /= (T t) noexcept { x /= t; y /= t; return *this; }
};
template<typename T>
constexpr Vector2<T> operator+(Vector2<T> lhs, const Vector2<T>& rhs) noexcept  {lhs += rhs;return lhs; }
template<typename T>
constexpr Vector2<T> operator/(Vector2<T> vec, T t) noexcept { vec /= t; return vec; }


template<typename T>
struct Box {
    T left, top, width, height;
    constexpr Box(T Left = 0, T Top = 0, T Width = 0, T Height = 0) noexcept 
        : left(Left), top(Top), width(Width), height(Height) {}
    constexpr Box(const Vector2<T>& position, const Vector2<T>& size) noexcept 
        : left(position.x), top(position.y), width(size.x), height(size.y) {}
    constexpr T getRight() const noexcept { return left + width; }
    constexpr T getBottom() const noexcept { return top + height; }
    constexpr Vector2<T> getTopLeft() const noexcept { return Vector2<T>(left, top); }
    constexpr Vector2<T> getCenter() const noexcept { return Vector2<T>(left + width / 2, top + height / 2); }
    constexpr Vector2<T> getSize() const noexcept { return Vector2<T>(width, height); }
    constexpr bool contains(const Box<T>& box) const noexcept {
        return left <= box.left && box.getRight() <= getRight() && top <= box.top && box.getBottom() <= getBottom();
    }
    constexpr bool containCenter(const Box<T>& box) const noexcept {
        auto center = box.getCenter();
        return center.x >= left && center.y >= top && center.x <= getRight() && center.y <= getBottom();
    }
    constexpr bool intersects(const Box<T>& box) const noexcept {
        return !(left >= box.getRight() || getRight() <= box.left || top >= box.getBottom() || getBottom() <= box.top);
    }
};

template<typename T>
struct Circle {
    Vector2<T> center;
    T radius;
    constexpr Circle(const Vector2<T>& c, T r) noexcept
        : center(c), radius(r) {}

    constexpr bool contains(const Vector2<T>& point) const noexcept {
        Vector2<T> offset = { point.x - center.x, point.y - center.y };
        return (offset.x * offset.x + offset.y * offset.y) <= (radius * radius);
    }
    constexpr bool contains(const Box<T>& box) const noexcept {
        auto l = box.left; auto r = box.getRight(); auto t = box.top; auto b = box.getBottom();
        return contains(Vector2{ l,t }) && contains(Vector2{ l,b }) && contains(Vector2{ r,t }) && contains(Vector2{ r,b });
    }
    constexpr bool containCenter(const Box<T>& box) const noexcept {
        return contains(box.getCenter());
    }
    constexpr bool intersects(const Box<T>& box) const noexcept {
        float closestX;
        if (center.x < box.left) {
            closestX = box.left;
        }
        else if (center.x > box.getRight()) {
            closestX = box.getRight();
        }
        else {
            closestX = center.x;
        }

        float closestY;
        if (center.y < box.top) {
            closestY = box.top;
        }
        else if (center.y > box.getBottom()) {
            closestY = box.getBottom();
        }
        else {
            closestY = center.y;
        }

        float dx = center.x - closestX;
        float dy = center.y - closestY;
        float distanceSquared = dx * dx + dy * dy;

        return distanceSquared <= (radius * radius);
    }
};

enum class QueryMode {
    Contain,
    ContainCenter,
    Intersect,
};

template<typename T, typename GetBox, typename Equal = std::equal_to<T>, typename Float = float>
class Quadtree
{
    static_assert(std::is_convertible_v<std::invoke_result_t<GetBox, const T&>, Box<Float>>,
        "GetBox must be a callable of signature Box<Float>(const T&)");
    static_assert(std::is_convertible_v<std::invoke_result_t<Equal, const T&, const T&>, bool>,
        "Equal must be a callable of signature bool(const T&, const T&)");
    static_assert(std::is_arithmetic_v<Float>);

public:
    Quadtree(const Box<Float>& box, const GetBox& getBox = GetBox(), const Equal& equal = Equal()) 
        : mBox(box), mRoot(std::make_unique<Node>()), mGetBox(getBox), mEqual(equal)
    {
        mRoot->box = box;
    }

    void add(const T& value)
    {
        add(mRoot.get(), 0, mBox, value);

        mAllValues.insert(value);
    }

    void remove(const T& value)
    {
        remove(mRoot.get(), mBox, value);

        mAllValues.erase(value);
    }

    void clear()
    {
        mAllValues.clear();
        mRoot = std::make_unique<Node>();
    }

    bool has(const T& value)
    {
        auto it = mAllValues.find(value);
        return it != mAllValues.end();
    }

    size_t count() const
    {
        return mAllValues.size();
    }

    QueryMode getQueryMode() 
    { 
        return mQueryMode; 
    }

    void setQueryMode(QueryMode mode) 
    { 
        mQueryMode = mode; 
    }

    std::vector<T> query(const Box<Float>& box) const
    {
        auto values = std::vector<T>();
        if (!getBox().contains(box)) return values;

        query(mRoot.get(), mBox, box, values);
        return values;
    }

    std::vector<T> query(const Circle<Float>& circle) const
    {
        auto values = std::vector<T>();

        query(mRoot.get(), mBox, circle, values);
        return values;
    }

    std::vector<std::pair<T, T>> findAllIntersections() const
    {
        auto intersections = std::vector<std::pair<T, T>>();
        findAllIntersections(mRoot.get(), intersections);
        return intersections;
    }

    Box<Float> getBox() const 
    {
        return mBox;
    }

public: 
    static constexpr auto Threshold = std::size_t(16);
    static constexpr auto MaxDepth = std::size_t(8);

    struct Node
    {
        std::array<std::unique_ptr<Node>, 4> children;
        std::vector<T> values;

        Box<float> box;
    };

    Node* getRoot() 
    { 
        return mRoot.get(); 
    }

private:
    Box<Float> mBox;
    std::unique_ptr<Node> mRoot;
    GetBox mGetBox;
    Equal mEqual;
    QueryMode mQueryMode = QueryMode::Intersect;

    std::set<T> mAllValues;

private:
    bool isLeaf(const Node* node) const
    {
        return !static_cast<bool>(node->children[0]);
    }

    Box<Float> computeBox(const Box<Float>& box, int i) const
    {
        auto origin = box.getTopLeft();
        auto childSize = box.getSize() / static_cast<Float>(2);
        switch (i)
        {
            // North West
            case 0:
                return Box<Float>(origin, childSize);
            // Norst East
            case 1:
                return Box<Float>(Vector2<Float>(origin.x + childSize.x, origin.y), childSize);
            // South West
            case 2:
                return Box<Float>(Vector2<Float>(origin.x, origin.y + childSize.y), childSize);
            // South East
            case 3:
                return Box<Float>(origin + childSize, childSize);
            default:
                assert(false && "Invalid child index");
                return Box<Float>();
        }
    }

    int getQuadrant(const Box<Float>& nodeBox, const Box<Float>& valueBox) const
    {
        auto center = nodeBox.getCenter();
        // West
        if (valueBox.getRight() < center.x)
        {
            // North West
            if (valueBox.getBottom() < center.y)
                return 0;
            // South West
            else if (valueBox.top >= center.y)
                return 2;
            // Not contained in any quadrant
            else
                return -1;
        }
        // East
        else if (valueBox.left >= center.x)
        {
            // North East
            if (valueBox.getBottom() < center.y)
                return 1;
            // South East
            else if (valueBox.top >= center.y)
                return 3;
            // Not contained in any quadrant
            else
                return -1;
        }
        // Not contained in any quadrant
        else
            return -1;
    }

    void add(Node* node, std::size_t depth, const Box<Float>& box, const T& value)
    {
        if (node == nullptr) return;
        if (!box.contains(mGetBox(value))) return;

        if (isLeaf(node))
        {
            // Insert the value in this node if possible
            if (depth >= MaxDepth || node->values.size() < Threshold)
                node->values.push_back(value);
            // Otherwise, we split and we try again
            else
            {
                split(node, box);
                add(node, depth, box, value);
            }
        }
        else
        {
            auto i = getQuadrant(box, mGetBox(value));
            // Add the value in a child if the value is entirely contained in it
            if (i != -1)
                add(node->children[static_cast<std::size_t>(i)].get(), depth + 1, computeBox(box, i), value);
            // Otherwise, we add the value in the current node
            else
                node->values.push_back(value);
        }

        node->box = box;
    }

    void split(Node* node, const Box<Float>& box)
    {
        assert(node != nullptr);
        assert(isLeaf(node) && "Only leaves can be split");
        
        // Create children
        for (auto& child : node->children)
            child = std::make_unique<Node>();

        // Assign values to children
        auto newValues = std::vector<T>(); // New values for this node
        for (const auto& value : node->values)
        {
            auto i = getQuadrant(box, mGetBox(value));
            if (i != -1)
                node->children[static_cast<std::size_t>(i)]->values.push_back(value);
            else
                newValues.push_back(value);
        }
        node->values = std::move(newValues);
    }

    bool remove(Node* node, const Box<Float>& box, const T& value)
    {
        if (node == nullptr) return false;
        if (!box.contains(mGetBox(value))) return false;

        if (isLeaf(node))
        {
            // Remove the value from node
            removeValue(node, value);
            return true;
        }
        else
        {
            // Remove the value in a child if the value is entirely contained in it
            auto i = getQuadrant(box, mGetBox(value));
            if (i != -1)
            {
                if (remove(node->children[static_cast<std::size_t>(i)].get(), computeBox(box, i), value))
                    return tryMerge(node);
            }
            // Otherwise, we remove the value from the current node
            else
                removeValue(node, value);
            return false;
        }
    }

    void removeValue(Node* node, const T& value)
    {
        // Find the value in node->values
        auto it = std::find_if(std::begin(node->values), std::end(node->values),
            [this, &value](const auto& rhs){ return mEqual(value, rhs); });
        assert(it != std::end(node->values) && "Trying to remove a value that is not present in the node");
        // Swap with the last element and pop back
        *it = std::move(node->values.back());
        node->values.pop_back();
    }

    bool tryMerge(Node* node)
    {
        assert(node != nullptr);
        assert(!isLeaf(node) && "Only interior nodes can be merged");
        auto nbValues = node->values.size();
        for (const auto& child : node->children)
        {
            if (!isLeaf(child.get()))
                return false;
            nbValues += child->values.size();
        }
        if (nbValues <= Threshold)
        {
            node->values.reserve(nbValues);
            // Merge the values of all the children
            for (const auto& child : node->children)
            {
                for (const auto& value : child->values)
                    node->values.push_back(value);
            }
            // Remove the children
            for (auto& child : node->children)
                child.reset();
            return true;
        }
        else
            return false;
    }

    void query(Node* node, const Box<Float>& box, const Box<Float>& queryBox, std::vector<T>& values) const
    {
        assert(node != nullptr);
        assert(queryBox.intersects(box));
        for (const auto& value : node->values)
        {
            if (mQueryMode == QueryMode::Contain)
            {
                if (queryBox.contains(mGetBox(value)))
                {
                    values.push_back(value);
                }
            }
            else if (mQueryMode == QueryMode::ContainCenter)
            {
                if (queryBox.containCenter(mGetBox(value)))
                {
                    values.push_back(value);
                }
            }
            else
            {
                if (queryBox.intersects(mGetBox(value)))
                {
                    values.push_back(value);
                }
            }
        }
        if (!isLeaf(node))
        {
            for (auto i = std::size_t(0); i < node->children.size(); ++i)
            {
                auto childBox = computeBox(box, static_cast<int>(i));
                if (queryBox.intersects(childBox))
                    query(node->children[i].get(), childBox, queryBox, values);
            }
        }
    }

    void query(Node* node, const Box<Float>& box, const Circle<Float>& queryCircle, std::vector<T>& values) const
    {
        assert(node != nullptr);
        assert(queryCircle.intersects(box));

        for (const auto& value : node->values)
        {
            if (mQueryMode == QueryMode::Contain)
            {
                if (queryCircle.contains(mGetBox(value)))
                {
                    values.push_back(value);
                }
            }
            else if (mQueryMode == QueryMode::ContainCenter)
            {
                if (queryCircle.containCenter(mGetBox(value)))
                {
                    values.push_back(value);
                }
            }
            else
            {
                if (queryCircle.intersects(mGetBox(value)))
                {
                    values.push_back(value);
                }
            }
        }

        if (!isLeaf(node))
        {
            for (auto i = std::size_t(0); i < node->children.size(); ++i)
            {
                auto childBox = computeBox(box, static_cast<int>(i));
                if (queryCircle.intersects(childBox))
                    query(node->children[i].get(), childBox, queryCircle, values);
            }
        }
    }
 
    void findAllIntersections(Node* node, std::vector<std::pair<T, T>>& intersections) const
    {
        // Find intersections between values stored in this node
        // Make sure to not report the same intersection twice
        for (auto i = std::size_t(0); i < node->values.size(); ++i)
        {
            for (auto j = std::size_t(0); j < i; ++j)
            {
                if (mGetBox(node->values[i]).intersects(mGetBox(node->values[j])))
                    intersections.emplace_back(node->values[i], node->values[j]);
            }
        }
        if (!isLeaf(node))
        {
            // Values in this node can intersect values in descendants
            for (const auto& child : node->children)
            {
                for (const auto& value : node->values)
                    findIntersectionsInDescendants(child.get(), value, intersections);
            }
            // Find intersections in children
            for (const auto& child : node->children)
                findAllIntersections(child.get(), intersections);
        }
    }

    void findIntersectionsInDescendants(Node* node, const T& value, std::vector<std::pair<T, T>>& intersections) const
    {
        // Test against the values stored in this node
        for (const auto& other : node->values)
        {
            if (mGetBox(value).intersects(mGetBox(other)))
                intersections.emplace_back(value, other);
        }
        // Test against values stored into descendants of this node
        if (!isLeaf(node))
        {
            for (const auto& child : node->children)
                findIntersectionsInDescendants(child.get(), value, intersections);
        }
    }
};

}
