
#pragma once
#include <SDL.h>
#include "Constants.h"

class Wall {
public:
    int x, y;
    SDL_Rect rect;
    bool active;
    SDL_Texture* texture;  // Thêm texture vào

    Wall(int startX, int startY, SDL_Texture* tex);
    void render(SDL_Renderer* renderer);
};

