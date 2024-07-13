#include "Astar.hpp"
#include <queue>
#include <map>
#include <cmath>
#include <algorithm>

Astar::Astar(const std::vector<std::vector<int>>& grid, std::pair<int, int> start, std::pair<int, int> end)
    : grid(grid), start(start), end(end) {}

std::vector<std::pair<int, int>> Astar::findPath() {
    std::map<std::pair<int, int>, std::pair<int, int>> cameFrom;
    std::map<std::pair<int, int>, int> costSoFar;
    
    auto compare = [](std::pair<int, std::pair<int, int>> a, std::pair<int, std::pair<int, int>> b) {
        return a.first > b.first;
    };
    
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, decltype(compare)> frontier(compare);

    frontier.push({0, start});
    cameFrom[start] = start;
    costSoFar[start] = 0;

    while (!frontier.empty()) {
        auto current = frontier.top().second;
        frontier.pop();

        if (current == end) {
            return reconstructPath(cameFrom);
        }

        for (auto next : getNeighbors(current)) {
            int newCost = costSoFar[current] + 1;
            if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next]) {
                costSoFar[next] = newCost;
                int priority = newCost + ManhattanHeuristic(next, end);
                frontier.push({priority, next});
                cameFrom[next] = current;
            }
        }
    }

    return std::vector<std::pair<int, int>>();
}

std::vector<std::pair<int, int>> Astar::reconstructPath(std::map<std::pair<int, int>, std::pair<int, int>>& cameFrom) {
    std::vector<std::pair<int, int>> path;
    auto current = end;
    while (current != start) {
        path.push_back(current);
        current = cameFrom[current];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

int Astar::ManhattanHeuristic(std::pair<int, int> a, std::pair<int, int> b) {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

std::vector<std::pair<int, int>> Astar::getNeighbors(std::pair<int, int> node) {
    std::vector<std::pair<int, int>> neighbors;
    std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto direction : directions) {
        int newX = node.first + direction.first;
        int newY = node.second + direction.second;
        if(direction.first != 0 && direction.second != 0) {
            if (newX >= 0 && newX < grid.size() && newY >= 0 && newY < grid[0].size() && grid[newX][newY] != 1) {
                if(grid[node.first + direction.first][node.second] != 1 && grid[node.first][node.second + direction.second] != 1) {
                    neighbors.push_back({newX, newY});
                }
            }
        }
        if(direction.first == 0 || direction.second == 0) {
            if (newX >= 0 && newX < grid.size() && newY >= 0 && newY < grid[0].size() && grid[newX][newY] != 1) {
                neighbors.push_back({newX, newY});
            }
        }
    }
    return neighbors;
}
