#include <SDL.h>
#include <vector>
#include <algorithm>
#include "Tank.h"
#include "constants.h"
#include "PlayerTank.h"
#include "Wall.h"

PlayerTank::PlayerTank(int startX, int startY) : Tank(startX, startY) {}

void PlayerTank::move(int dx, int dy, const std::vector<Wall>& walls) {
    const int newX = x + dx;
    const int newY = y + dy;
    dirX = dx;
    dirY = dy;
    const SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
            return;
        }
    }
    if (newX >= 0 && newX + TILE_SIZE <= SCREEN_WIDTH &&
        newY >= 0 && newY + TILE_SIZE <= SCREEN_HEIGHT) {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
    }
}

void PlayerTank::render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Texture* bulletTexture) const {
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    for (const auto& bullet : bullets) {
        bullet.render(renderer, bulletTexture);
    }
}
