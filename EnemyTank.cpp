#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include "Tank.h"
#include "constants.h"
#include "EnemyTank.h"
#include "Bullet.h"
#include"EnemyTank.h"

void EnemyTank::move() {
    if (rand() % 50 == 0) {
        int r = rand() % 4;
        switch (r) {
            case 0: dirX = 0;  dirY = -1; break;
            case 1: dirX = 1;  dirY = 0;  break;
            case 2: dirX = 0;  dirY = 1;  break;
            case 3: dirX = -1; dirY = 0;  break;
        }
    }
    x += dirX * 2;
    y += dirY * 2;
    rect.x = x;
    rect.y = y;
}

void EnemyTank::render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Texture* bulletTexture) const {
    if (active) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        for (const auto& bullet : bullets) {
            bullet.render(renderer, bulletTexture);
        }
    }
}
