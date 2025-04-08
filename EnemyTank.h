#pragma once
#include "Tank.h"
#include "Bullet.h"
#include <vector>
class EnemyTank : public Tank {
public:
    bool active;

    EnemyTank(int startX, int startY) : Tank(startX, startY) {
        active = true;
    }

    void move();

    void render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Texture* bulletTexture) const;
};
