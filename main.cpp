#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include<Wall.h>
#include "constants.h"
using namespace std;
class Bullet {
public:
    int x, y;
    int dx, dy;
    SDL_Rect rect;
    bool active;

    Bullet(int startX, int startY, int dirX, int dirY) {
        x = startX;
        y = startY;
        dx = dirX * 5;
        dy = dirY * 5;
        active = true;
        rect = {x, y, 10, 10};
    }

    void move() {
        x += dx;
        y += dy;
        rect.x = x;
        rect.y = y;
        if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT) {
            active = false;
        }
    }

    void render(SDL_Renderer* renderer, SDL_Texture* texture) const {
        if (active) {
            SDL_RenderCopy(renderer, texture, NULL, &rect);
        }
    }
};

class Tank {
public:
    int x, y;
    int dirX, dirY;
    SDL_Rect rect;
    vector<Bullet> bullets;

    Tank(int startX, int startY) {
        x = startX;
        y = startY;
        rect = {x, y, TILE_SIZE, TILE_SIZE};
        dirX = 0;
        dirY = -1;
    }

    void shoot() {
        bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY));
    }

    void updateBullets() {
        for (auto& bullet : bullets) {
            bullet.move();
        }
        bullets.erase(remove_if(bullets.begin(), bullets.end(),
                                [](Bullet& b) { return !b.active; }), bullets.end());
    }
};

class PlayerTank : public Tank {
public:
    PlayerTank(int startX, int startY) : Tank(startX, startY) {}

    void move(int dx, int dy, const vector<Wall>& walls) {
        int newX = x + dx;
        int newY = y + dy;
        dirX = dx;
        dirY = dy;

        SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
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

    void render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Texture* bulletTexture) const {
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        for (const auto& bullet : bullets) {
            bullet.render(renderer, bulletTexture);
        }
    }
};

class EnemyTank : public Tank {
public:
    bool active;

    EnemyTank(int startX, int startY) : Tank(startX, startY) {
        active = true;
    }

    void move() {
        if (rand() % 50 == 0) {
            int r = rand() % 4;
            if (r == 0) { dirX = 0; dirY = -1; }
            else if (r == 1) { dirX = 1; dirY = 0; }
            else if (r == 2) { dirX = 0; dirY = 1; }
            else if (r == 3) { dirX = -1; dirY = 0; }
        }

        x += dirX * 2;
        y += dirY * 2;
        rect.x = x;
        rect.y = y;
    }

    void render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Texture* bulletTexture) const {
        if (active) {
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            for (const auto& bullet : bullets) {
                bullet.render(renderer, bulletTexture);
            }
        }
    }
};

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    vector<Wall> walls;
    PlayerTank player;
    vector<EnemyTank> enemies;
    void spawnEnemies();
    SDL_Texture* playerTexture;
    SDL_Texture* enemyTexture;
    SDL_Texture* bulletTexture;
    SDL_Texture* wallTexture;
    SDL_Texture* backgroundTexture;

    Game() : player(SCREEN_WIDTH / 2, SCREEN_HEIGHT - TILE_SIZE * 2) {
        running = true;
        SDL_Init(SDL_INIT_VIDEO);
        IMG_Init(IMG_INIT_PNG);
        window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        playerTexture = IMG_LoadTexture(renderer, "player.png");
        enemyTexture = IMG_LoadTexture(renderer, "enemy.png");
        bulletTexture = IMG_LoadTexture(renderer, "bullet.png");
        wallTexture = IMG_LoadTexture(renderer, "wall.png");
        backgroundTexture = IMG_LoadTexture(renderer, "background.png");

        generateWalls();
        spawnEnemies();
    }

    void generateWalls() {
        for (int i = 3; i < MAP_HEIGHT - 3; i += 2) {
            for (int j = 3; j < MAP_WIDTH - 3; j += 2) {
                walls.push_back(Wall(j * TILE_SIZE, i * TILE_SIZE));
            }
        }
    }

    void render() {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        for (const auto& wall : walls) {
            wall.render(renderer, wallTexture);
        }
        player.render(renderer, playerTexture, bulletTexture);
        for (auto& enemy : enemies) {
            enemy.render(renderer, enemyTexture, bulletTexture);
        }
        SDL_RenderPresent(renderer);
    }

    void run() {
        while (running) {
            handleEvents();
            update();
            render();
            SDL_Delay(16);
        }
    }

    void update() {
        player.updateBullets();
        for (auto& enemy : enemies) {
            enemy.move();
            enemy.updateBullets();
            if (rand() % 100 < 2) {
                enemy.shoot();
            }
        }
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: player.move(0, -5, walls); break;
                    case SDLK_DOWN: player.move(0, 5, walls); break;
                    case SDLK_LEFT: player.move(-5, 0, walls); break;
                    case SDLK_RIGHT: player.move(5, 0, walls); break;
                    case SDLK_SPACE: player.shoot(); break;
                }
            }
        }
    }
};

int main() {
    srand(time(0));
    Game game;
    if (game.running) {
        game.run();
    }
    return 0;
}
