#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <SDL_image.h>
#include <algorithm>
#include "Tank.h"
#include "constants.h"
#include "EnemyTank.h"
#include "Bullet.h"
#include"EnemyTank.h"
#include"Game.h"
    Game::Game() : player(SCREEN_WIDTH / 2, SCREEN_HEIGHT - TILE_SIZE * 2) {
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

    void Game::spawnEnemies() {
        this->enemies.clear();
        for(int i=0;i<this->enemyNumber;i++)
        {
            int ex,ey;
            bool validPosition = false;
            while(!validPosition)
            {
                ex= (rand()%(MAP_WIDTH-2)+1)*TILE_SIZE;
                ey= (rand()%(MAP_HEIGHT-2)+1)*TILE_SIZE;
                validPosition = true;
                for(const auto & wall :walls)
                {
                    if(wall.active && wall.x == ex && wall.y==ey)
                    {
                        validPosition = false;
                        break;
                    }
                }
                this->enemies.push_back(EnemyTank(ex, ey));
            }
        }
    }


    void Game::generateWalls() {
        for (int i = 3; i < MAP_HEIGHT - 3; i += 2) {
            for (int j = 3; j < MAP_WIDTH - 3; j += 2) {
                walls.push_back(Wall(j * TILE_SIZE, i * TILE_SIZE));
            }
        }
    }

    void Game::render() {
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

    void Game::run() {
        while (running) {
            handleEvents();
            update();
            render();
            SDL_Delay(16);
        }
    }

    void Game::update() {
        player.updateBullets();
        for (auto& enemy : enemies) {
            enemy.move();
            enemy.updateBullets();
            if (rand() % 100 < 2) {
                enemy.shoot();
            }
        }
    }

    void Game::handleEvents() {
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

