#include "Bullet.h"
#include "constants.h"

Bullet::Bullet(int startX, int startY, int dirX, int dirY)
{
    x = startX;
    y = startY;
    dx = dirX * 5;
    dy = dirY * 5;
    active = true;
    rect.x = x;
    rect.y = y;
    rect.w = 10;
    rect.h = 10;
}

void Bullet::move()
{
    x += dx;
    y += dy;
    rect.x = x;
    rect.y = y;
    if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT)
    {
        active = false;
    }
}

void Bullet::render(SDL_Renderer* renderer, SDL_Texture* texture)
const {
    if (active)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}
