#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "Astar.hpp"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void run();

private:
    void handleEvents();
    void render();
    void drawGrid();
    void runPathfinding();
    void resetGrid();
    void clearPath();
    void moveEndPoint();

    void moveStartNode(int deltaX, int deltaY);
    bool isValidMove(int newX, int newY) const;

    SDL_Window *window;
    SDL_Renderer *renderer;
    std::vector<std::vector<int>> grid;
    std::vector<std::pair<int, int>> path;
    std::pair<int, int> start;
    std::pair<int, int> end;
    static const int WINDOW_WIDTH;
    static const int WINDOW_HEIGHT;
    static const int GRID_SIZE;
    static const int MOVE_INTERVAL_MS;
    Uint32 lastMoveTime;
    bool isRunning;
    bool isDrawing;
    bool hasPath;
    bool moveEnd;
    bool endpointPaused;
};
