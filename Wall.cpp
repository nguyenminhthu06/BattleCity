#include"Wall.h"

 Wall::Wall(int startX, int startY)
{
    x = startX;
    y = startY;
    active = true;
    rect = {x,y,TILE_SIZE, TILE_SIZE};
}

void Wall::render(SDL_Renderer* renderer)
{
    if(active)
    {
        SDL_SetRenderDrawColor(renderer, 251, 193, 147, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}
