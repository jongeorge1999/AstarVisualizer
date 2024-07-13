#include "Renderer.hpp"
#include "Astar.hpp"

const int Renderer::WINDOW_WIDTH = 800;
const int Renderer::WINDOW_HEIGHT = 600;
const int Renderer::GRID_SIZE = 5;
const int Renderer::MOVE_INTERVAL_MS = 100;

Renderer::Renderer() : window(nullptr), renderer(nullptr), isRunning(true), isDrawing(false), moveEnd(false), lastMoveTime(0), endpointPaused(false), hasPath(false) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Visual A*", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    grid.resize(WINDOW_WIDTH / GRID_SIZE, std::vector<int>(WINDOW_HEIGHT / GRID_SIZE, 0));
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Renderer::run() {
    while (isRunning) {
        handleEvents();
        render();
        if (moveEnd && !endpointPaused && SDL_GetTicks() - lastMoveTime >= MOVE_INTERVAL_MS) {
            moveEndPoint();
            lastMoveTime = SDL_GetTicks();
        }
        SDL_Delay(2);
    }
}

void Renderer::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            int gridX = mouseX / GRID_SIZE;
            int gridY = mouseY / GRID_SIZE;
            if (gridX >= 0 && gridX < WINDOW_WIDTH / GRID_SIZE && gridY >= 0 && gridY < WINDOW_HEIGHT / GRID_SIZE) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isDrawing = true;
                    if (grid[gridX][gridY] == 0) {
                        grid[gridX][gridY] = 1;
                    }
                } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                    if (grid[gridX][gridY] == 0) {
                        grid[gridX][gridY] = 2;
                        start = {gridX, gridY};
                    }
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    if (grid[gridX][gridY] == 0) {
                        grid[gridX][gridY] = 3;
                        end = {gridX, gridY};
                    }
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                isDrawing = false;
            }
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_o:
                    endpointPaused = !endpointPaused;
                    break;
                case SDLK_p:
                    hasPath = true;
                    runPathfinding();
                    if(endpointPaused) {
                        endpointPaused = !endpointPaused;
                    }
                    break;
                case SDLK_c:
                    hasPath = false;
                    clearPath();
                    if(!endpointPaused) {
                        endpointPaused = !endpointPaused;
                    }
                    break;
                case SDLK_r:
                    resetGrid();
                    break;
                case SDLK_UP:
                    moveStartNode(0, -1);
                    break;
                case SDLK_DOWN:
                    moveStartNode(0, 1);
                    break;
                case SDLK_LEFT:
                    moveStartNode(-1, 0);
                    break;
                case SDLK_RIGHT:
                    moveStartNode(1, 0);
                    break;
                default:
                    break;
            }
        }

        if (isDrawing && event.type == SDL_MOUSEMOTION) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            int gridX = mouseX / GRID_SIZE;
            int gridY = mouseY / GRID_SIZE;
            if (gridX >= 0 && gridX < WINDOW_WIDTH / GRID_SIZE && gridY >= 0 && gridY < WINDOW_HEIGHT / GRID_SIZE) {
                if (grid[gridX][gridY] == 0) {
                    grid[gridX][gridY] = 1;
                }
            }
        }
    }
}

void Renderer::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    drawGrid();
    SDL_RenderPresent(renderer);
}

void Renderer::drawGrid() {
    for (int x = 0; x < WINDOW_WIDTH / GRID_SIZE; x++) {
        for (int y = 0; y < WINDOW_HEIGHT / GRID_SIZE; y++) {
            SDL_Rect rect = { x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
            switch (grid[x][y]) {
                case 1:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                case 2:
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                case 3:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                case 4:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &rect);
                    break;
            }
        }
    }
}

void Renderer::resetGrid() {
    grid.assign(WINDOW_WIDTH / GRID_SIZE, std::vector<int>(WINDOW_HEIGHT / GRID_SIZE, 0));
    start = {-1, -1};
    end = {-1, -1};
}

void Renderer::runPathfinding() {
    clearPath();

    Astar astar(grid, start, end);
    path = astar.findPath();

    for (auto point : path) {
        if (grid[point.first][point.second] == 0) {
            grid[point.first][point.second] = 4;
        }
    }

    moveEnd = true;
    lastMoveTime = SDL_GetTicks();
}

void Renderer::moveStartNode(int deltaX, int deltaY) {
    int newX = start.first + deltaX;
    int newY = start.second + deltaY;
    if (isValidMove(newX, newY)) {
        grid[start.first][start.second] = 0;
        start.first = newX;
        start.second = newY;
        grid[start.first][start.second] = 2;
        if(hasPath) {
            runPathfinding();
        }
    }
}

void Renderer::moveEndPoint() {
    if (!path.empty()) {
        int nextX = path.back().first;
        int nextY = path.back().second;
        grid[end.first][end.second] = 0;
        end.first = nextX;
        end.second = nextY;
        grid[end.first][end.second] = 3;
        path.pop_back();

        if (end == start) {
            moveEnd = false;
        }

        render();
    }
}

bool Renderer::isValidMove(int newX, int newY) const {
    return newX >= 0 && newX < WINDOW_WIDTH / GRID_SIZE &&
           newY >= 0 && newY < WINDOW_HEIGHT / GRID_SIZE &&
           grid[newX][newY] != 1 && grid[newX][newY] != 2;
}

void Renderer::clearPath() {
    for (int x = 0; x < WINDOW_WIDTH / GRID_SIZE; x++) {
        for (int y = 0; y < WINDOW_HEIGHT / GRID_SIZE; y++) {
            if (grid[x][y] == 4) {
                grid[x][y] = 0;
            }
        }
    }
}