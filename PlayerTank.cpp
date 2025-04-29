#include "PlayerTank.h"
#include <algorithm>
#include <iostream>
#include <SDL_image.h>

PlayerTank::PlayerTank(int startX, int startY, SDL_Renderer* renderer) {
    x = startX;
    y = startY;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    dirX = 0;
    dirY = -1;
    speed = 3.0f;
    texture = nullptr;
    loadTexture(renderer, "D:/A_Teaching/LTNC/2024/DEMO/playertank.jpg");  // Sử dụng đường dẫn tương đối
}

PlayerTank::~PlayerTank() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void PlayerTank::loadTexture(SDL_Renderer* renderer, const char* path) {
    if (texture) {
        SDL_DestroyTexture(texture);
    }

    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "Failed to load tank texture: " << IMG_GetError() << std::endl;
        texture = nullptr;
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
    }
}

void PlayerTank::move(int dx, int dy, const std::vector<Wall>& walls) {
    float newX = x + dx * speed;
    float newY = y + dy * speed;
    this->dirX = dx;
    this->dirY = dy;

    // Sửa cảnh báo narrowing conversion
    SDL_Rect newRect = {
        static_cast<int>(newX),
        static_cast<int>(newY),
        TILE_SIZE,
        TILE_SIZE
    };

    // Kiểm tra va chạm với tường
    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
            return;
        }
    }

    if(newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE*2 &&
       newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE*2) {
        x = static_cast<int>(newX);
        y = static_cast<int>(newY);
        rect.x = x;
        rect.y = y;
    }
}

void PlayerTank::shoot() {
    bullets.emplace_back(x + TILE_SIZE/2 - 5, y + TILE_SIZE/2 - 5, this->dirX, this->dirY);
}

void PlayerTank::updateBullets() {
    for (auto& bullet : bullets) {
        bullet.move();
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](Bullet& b) { return !b.active; }), bullets.end());
}

void PlayerTank::render(SDL_Renderer* renderer) {
    if (texture) {
        // Xác định góc quay dựa trên hướng
        double angle = 0;
        if (dirX == 1) angle = 90;
        else if (dirX == -1) angle = 270;
        else if (dirY == 1) angle = 180;

        SDL_RenderCopyEx(renderer, texture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
    } else {
        // Fallback nếu không có texture
        SDL_SetRenderDrawColor(renderer, 255, 182, 175, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    // Render đạn
    for (auto& bullet : bullets) {
        bullet.render(renderer);
    }
}

void PlayerTank::setSpeed(float newSpeed) {
    speed = newSpeed;
}
