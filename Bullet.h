#pragma once
#include <SDL.h>
class Bullet {
public:
    int x, y;
    int dx, dy;
    SDL_Rect rect;
    bool active;
    Bullet(int startX, int startY, int dirX, int dirY);
    void move();
    void render(SDL_Renderer* renderer, SDL_Texture* texture) const;
};
