/*
    Copyright (c) 2015, Damian Barczynski <daan.net@wp.eu>
    Following tool is licensed under the terms and conditions of the ISC license.
    For more information visit https://opensource.org/licenses/ISC.
*/
#ifndef __ASTAR_HPP_8F637DB91972F6C878D41D63F7E7214F__
#define __ASTAR_HPP_8F637DB91972F6C878D41D63F7E7214F__

#include <set>
#include <vector>
#include <functional>

namespace AStar
{
    struct Vec2i
    {
        int x, y;

        bool operator == (const Vec2i& coordinates_) const {
            return (x == coordinates_.x && y == coordinates_.y);
        }

        Vec2i operator + (const AStar::Vec2i& right_) const {
            return{ x + right_.x, y + right_.y };
        }
    };

    using uint = unsigned int;
    using HeuristicFunction = std::function<uint(const Vec2i&, const Vec2i&)>;
    using CoordinateList = std::vector<Vec2i>;

    struct Node
    {
        uint G, H;
        Vec2i coordinates;
        Node *parent;

        Node(const Vec2i& coord_, Node *parent_ = nullptr) {
            parent = parent_;
            coordinates = coord_;
            G = H = 0;
        }

        uint getScore(){ return G + H; }
    };

    using NodeSet = std::vector<Node*>;


    class Heuristic
    {
        static Vec2i getDelta(const Vec2i& source_, const Vec2i& target_) {
            return{ abs(source_.x - target_.x),  abs(source_.y - target_.y) };
        }

    public:
        static uint manhattan(const Vec2i& source_, const Vec2i& target_) {
            auto delta = std::move(getDelta(source_, target_));
            return static_cast<uint>(10 * (delta.x + delta.y));
        }

        static uint euclidean(const Vec2i& source_, const Vec2i& target_) {
            auto delta = std::move(getDelta(source_, target_));
            return static_cast<uint>(10 * sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
        }

        static uint octagonal(const Vec2i& source_, const Vec2i& target_) {
            auto delta = std::move(getDelta(source_, target_));
            return 10 * (delta.x + delta.y) + (-6) * std::min(delta.x, delta.y);
        }
    };


    class Generator
    {
        bool detectCollision(const Vec2i& coordinates_) {
            if (coordinates_.x < 0 || coordinates_.x >= worldSize.x ||
                coordinates_.y < 0 || coordinates_.y >= worldSize.y ||
                std::find(walls.begin(), walls.end(), coordinates_) != walls.end()) {
                return true;
            }
            return false;
        }

        Node* findNodeOnList(NodeSet& nodes_, const Vec2i& coordinates_) {
            for (auto node : nodes_) {
                if (node->coordinates == coordinates_) {
                    return node;
                }
            }
            return nullptr;
        }

        void releaseNodes(NodeSet& nodes_) {
            for (auto it = nodes_.begin(); it != nodes_.end();) {
                delete* it;
                it = nodes_.erase(it);
            }
        }

    public:
        Generator() {
            setDiagonalMovement(false);
            setHeuristic(&Heuristic::manhattan);
            direction = {
                { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
                { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
            };
        }

        void setWorldSize(const Vec2i& worldSize_) {
            worldSize = worldSize_;
        }

        void setDiagonalMovement(bool enable_) {
            directions = (enable_ ? 8 : 4);
        }

        void setHeuristic(HeuristicFunction heuristic_) {
            //heuristic = std::bind(heuristic_, _1, _2);
            heuristic = heuristic_;
        }

        CoordinateList findPath(const Vec2i& source_, const Vec2i& target_) {
            Node* current = nullptr;
            NodeSet openSet, closedSet;
            openSet.reserve(100);
            closedSet.reserve(100);
            openSet.push_back(new Node(source_));

            while (!openSet.empty()) {
                auto current_it = openSet.begin();
                current = *current_it;

                for (auto it = openSet.begin(); it != openSet.end(); it++) {
                    auto node = *it;
                    if (node->getScore() <= current->getScore()) {
                        current = node;
                        current_it = it;
                    }
                }

                if (current->coordinates == target_) {
                    break;
                }

                closedSet.push_back(current);
                openSet.erase(current_it);

                for (uint i = 0; i < directions; ++i) {
                    Vec2i newCoordinates(current->coordinates + direction[i]);
                    if (detectCollision(newCoordinates) ||
                        findNodeOnList(closedSet, newCoordinates)) {
                        continue;
                    }

                    uint totalCost = current->G + ((i < 4) ? 10 : 14);

                    Node* successor = findNodeOnList(openSet, newCoordinates);
                    if (successor == nullptr) {
                        successor = new Node(newCoordinates, current);
                        successor->G = totalCost;
                        successor->H = heuristic(successor->coordinates, target_);
                        openSet.push_back(successor);
                    }
                    else if (totalCost < successor->G) {
                        successor->parent = current;
                        successor->G = totalCost;
                    }
                }
            }

            CoordinateList path;
            while (current != nullptr) {
                path.push_back(current->coordinates);
                current = current->parent;
            }

            releaseNodes(openSet);
            releaseNodes(closedSet);

            return path;
        }

        void addCollision(const Vec2i& coordinates_) {
            walls.push_back(coordinates_);
        }

        void removeCollision(const Vec2i& coordinates_) {
            auto it = std::find(walls.begin(), walls.end(), coordinates_);
            if (it != walls.end()) {
                walls.erase(it);
            }
        }

        void clearCollisions() {
            walls.clear();
        }

    private:
        HeuristicFunction heuristic;
        CoordinateList direction, walls;
        Vec2i worldSize;
        uint directions;
    };

}

#endif // __ASTAR_HPP_8F637DB91972F6C878D41D63F7E7214F__
