#include"Wall.h"

Wall::Wall(int startX, int startY, SDL_Texture* tex) {
    x = startX;
    y = startY;
    rect = { x, y, TILE_SIZE*2, TILE_SIZE*2};
    texture = tex;
    active = true;
}

void Wall::render(SDL_Renderer* renderer) const
{
    if (!active) return;
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
    else{
        SDL_SetRenderDrawColor(renderer, 251, 193, 147, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}



