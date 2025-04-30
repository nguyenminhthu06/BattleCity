#include "Wall.h"

Wall::Wall(int startX, int startY, SDL_Texture* tex)
{
    x = startX;
    y = startY;
    active = true;
    rect = { x, y, 200, 200};  // hoặc TILE_SIZE nếu bạn dùng constant
    texture = tex;
}

void Wall::render(SDL_Renderer* renderer) {
    if (active && texture) {
        SDL_Rect destRect = {x, y, 100, 100};
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }
}

