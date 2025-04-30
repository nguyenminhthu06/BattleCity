
#pragma once
#include <SDL.h>
#include "Constants.h"

class Wall {
public:
    int x, y;
    SDL_Rect rect;
    bool active;
    SDL_Texture* texture;  // Th�m texture v�o

    Wall(int startX, int startY, SDL_Texture* tex);
    void render(SDL_Renderer* renderer);
};

