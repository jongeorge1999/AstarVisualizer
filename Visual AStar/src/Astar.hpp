#pragma once

#include <vector>
#include <map>

class Astar {
public:
    Astar(const std::vector<std::vector<int>>& grid, std::pair<int, int> start, std::pair<int, int> end);
    std::vector<std::pair<int, int>> findPath();

private:
    std::vector<std::vector<int>> grid;
    std::pair<int, int> start;
    std::pair<int, int> end;
    std::vector<std::pair<int, int>> reconstructPath(std::map<std::pair<int, int>, std::pair<int, int>>& cameFrom);
    int ManhattanHeuristic(std::pair<int, int> a, std::pair<int, int> b);
    std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> node);
};
