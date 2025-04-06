#include "Wall.h"
#include "constants.h"

Wall::Wall(int startX, int startY) {
    x = startX;
    y = startY;
    active = true;
    rect.x = x;
    rect.y = y;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
}

void Wall::render(SDL_Renderer* renderer, SDL_Texture* texture)
const {
    if (active)
    {
        SDL_SetRenderDrawColor(renderer, 150, 70, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}
