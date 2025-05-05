#include "PlayerTank.h"
#include <iostream>
#include <algorithm>

PlayerTank::PlayerTank(int startX, int startY) {
    x = startX;
    y = startY;
    rect = { x, y, TILE_SIZE*2, TILE_SIZE*2};
    dirX = 0;
    dirY = -1;
    speed = 5.0f;
}
void PlayerTank::setPosition(int x, int y) {
        this->x = x;
        this->y = y;
        rect.x = x;
        rect.y = y;
    }
void PlayerTank::move(int dx, int dy, const std::vector<Wall>& walls) {
    float newX = x + dx * speed;
    float newY = y + dy * speed;
    dirX = dx;
    dirY = dy;

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

    // Cập nhật vị trí nếu không va chạm và trong phạm vi màn hình
    if (newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 &&
        newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2) {
        x = static_cast<int>(newX);
        y = static_cast<int>(newY);
        rect.x = x;
        rect.y = y;
    }
}

void PlayerTank::shoot() {
    bullets.emplace_back(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY);
}

void PlayerTank::updateBullets() {
    for (auto& bullet : bullets) {
        bullet.move();
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](Bullet& b) { return !b.active; }), bullets.end());
}

PlayerTank::~PlayerTank() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

bool PlayerTank::loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cout << "Failed to load tank image: " << IMG_GetError() << std::endl;
        return false;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cout << "Failed to create tank texture: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void PlayerTank::setTexture(SDL_Texture* tex) {
    // Nếu đã có texture cũ, giải phóng trước khi gán
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    texture = tex;
}

void PlayerTank::render(SDL_Renderer* renderer) {
    // Vẽ texture nếu có
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    } else {
        // Nếu không có texture thì render màu fallback
        SDL_SetRenderDrawColor(renderer, 255, 182, 175, 255);  // fallback color
        SDL_RenderFillRect(renderer, &rect);
    }

    // Render các viên đạn
    for (auto& bullet : bullets) {
        bullet.render(renderer);
    }
}

void PlayerTank::setSpeed(float newSpeed) {
    speed = newSpeed;
}


void PlayerTank::update(float deltaTime) {
    updateBullets();
    // Bạn có thể thêm animation hoặc logic khác tại đây nếu cần
}

