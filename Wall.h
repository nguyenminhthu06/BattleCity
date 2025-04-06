#pragma once
#include <SDL.h>
class Wall {
public:
    int x, y;
    SDL_Rect rect;
    bool active;
    Wall(int startX, int startY);
    void render(SDL_Renderer* renderer, SDL_Texture* texture)const;
};
