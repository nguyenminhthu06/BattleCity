#include "EnemyTank.h"
#include <cstdlib>
#include <algorithm>
#include <vector>

EnemyTank::EnemyTank(int startX, int startY, float speed) {
    x = startX;
    y = startY;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    dirX = 0;
    dirY = 1;
    active = true;

    // Khởi tạo các giá trị tốc độ
    moveSpeed = speed;
    moveDelay = static_cast<int>(15 / moveSpeed); // Điều chỉnh delay dựa trên tốc độ
    shootDelay = 3; // Thời gian delay giữa các lần bắn
    bulletSpeed = 1.0f;; // Tốc độ đạn mặc định
}

void EnemyTank::setSpeed(float newSpeed) {
    moveSpeed = newSpeed;
    moveDelay = static_cast<int>(15 / moveSpeed);
}

void EnemyTank::setBulletSpeed(float newSpeed) {
    bulletSpeed = newSpeed;
}

void EnemyTank::move(const std::vector<Wall>& walls) {
    if (--moveDelay > 0) return;
    moveDelay = static_cast<int>(15 / moveSpeed); // Cập nhật delay dựa trên tốc độ

    // Chọn hướng di chuyển ngẫu nhiên
    int r = rand() % 4;
    int moveStep = static_cast<int>(9 * moveSpeed); // Bước di chuyển tỷ lệ với tốc độ

    if (r == 0) {
        dirX = 0;
        dirY = -moveStep;
    }
    else if (r == 1) {
        dirX = 0;
        dirY = moveStep;
    }
    else if (r == 2) {
        dirX = -moveStep;
        dirY = 0;
    }
    else if (r == 3) {
        dirX = moveStep;
        dirY = 0;
    }

    int newX = x + dirX;
    int newY = y + dirY;

    SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
            return;
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

void EnemyTank::shoot() {
    if (--shootDelay > 0) return;
    shootDelay = 3; // Reset delay bắn

    // Tạo đạn với tốc độ đã được điều chỉnh
    bullets.emplace_back(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5,
                        dirX * bulletSpeed, dirY * bulletSpeed);
}

void EnemyTank::updateBullets() {
    for (auto& bullet : bullets) {
        bullet.move();
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](Bullet& b) { return !b.active; }), bullets.end());
}

void EnemyTank::render(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 242, 128, 118, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Vẽ đạn
    for (auto& bullet : bullets) {
        bullet.render(renderer);
    }
}
