#pragma once
#include <SDL.h>
#include <vector>
#include<string>
#include "PlayerTank.h"
#include "Wall.h"
#include "EnemyTank.h"
#include "Constants.h"

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool inMenu;

    // Game objects
    std::vector<Wall> walls;
    PlayerTank player;
    std::vector<EnemyTank> enemies;
    int enemyNumber = 3;

    // Textures
    SDL_Texture* menuBackgroundTexture;
    SDL_Texture* instructionTexture;
    SDL_Texture* instructionBgTexture;
    SDL_Texture* gameOverTexture;
    SDL_Texture* victoryTexture;

    // Game state
    enum class GameState {
        MENU,
        INSTRUCTIONS,
        PLAYING,
        GAME_OVER,
        VICTORY
    };
    GameState state;

    // Button structure
    struct Button {
        SDL_Rect rect;
        SDL_Texture* texture;
        SDL_Texture* hoverTexture;
        bool isHovered = false;
    };

    Button playButtonMenu;
    Button playButtonGame;
    Button retryButton;
    Button menuButton;

    // Methods
    Game();
    ~Game();

    SDL_Texture* loadTexture(const std::string& path);
    void run();
    void handleEvents();
    void update();
    void render();

    void initMenu();
    void generateWalls();
    void spawnEnemies();

    void handleMenuEvents();
    void handleInstructionEvents();
    void renderMenu();
    void renderInstructions();
    void handleGameOverEvents();
    void handleVictoryEvents();

    void renderGameOver();
    void renderVictory();
    void handleEndScreenEvents();
    void resetGame();
};
