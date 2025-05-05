#include "EnemyTank.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include "Bullet.h"
#include "Wall.h"

EnemyTank::EnemyTank(int startX, int startY) {
    x = startX;
    y = startY;
    rect = {x, y, TILE_SIZE*2, TILE_SIZE*2};
    dirX = 0;
    dirY = 1;
    active = true;
    moveDelay = static_cast<int>(10 / moveSpeed);
    shootDelay = 1;
    moveSpeed=2.0f;
    texture = nullptr;
}

void EnemyTank::move(const std::vector<Wall>& walls) {
    if (--moveDelay > 0) return;
    moveDelay = static_cast<int>(10 / moveSpeed);
    int r = rand() % 4;
    int moveStep = static_cast<int>(4 * moveSpeed);
    if (r == 0) {
        this->dirX = 0;
        this->dirY = -moveStep;
    }
    else if (r == 1) {
        this->dirX = 0;
        this->dirY = moveStep;
    }
    else if (r == 2) {
        this->dirX = -moveStep;
        this->dirY = 0;
    }
    else if (r == 3) {
        this->dirX = moveStep;
        this->dirY = 0;
    }

    int newX = x + this->dirX;
    int newY = y + this->dirY;

    SDL_Rect newRect = { newX, newY, TILE_SIZE, TILE_SIZE };
    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
            return; // Tránh va chạm với tường
        }
    }

    if (newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 &&
        newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2) {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
    }
}
void EnemyTank::setSpeed(float speed) {
    moveSpeed = speed;
    moveDelay = static_cast<int>(10 / moveSpeed);
}

void EnemyTank::shoot() {
    if (--shootDelay > 0) return;
    shootDelay = 1;

    bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5,
                         this->dirX, this->dirY));
}

void EnemyTank::updateBullets() {
    for (auto& bullet : bullets) {
        bullet.move();
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](Bullet& b) { return !b.active; }), bullets.end());
}

void EnemyTank::render(SDL_Renderer* renderer) const {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    for (const auto& bullet : bullets)
        bullet.render(renderer);
}

void EnemyTank::setTexture(SDL_Texture* tex) {
    texture = tex;
}
void EnemyTank::update(float deltaTime, const std::vector<Wall>& walls) {
    moveDelay--;
    shootDelay--;
    moveDelay = std::max(0, moveDelay);
    shootDelay = std::max(0, shootDelay);

    move(walls);
    updateBullets();
}





