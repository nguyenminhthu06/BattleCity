#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include <vector>
#include<string>
#include "PlayerTank.h"
#include "Wall.h"
#include "EnemyTank.h"
#include "Constants.h"
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include"Audio.h"

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    bool running;
    bool inMenu;

    std::vector<Wall> walls;
    PlayerTank player;
    std::vector<EnemyTank> enemies;
    int enemyNumber = 3;

    SDL_Texture* menuBackgroundTexture;
    SDL_Texture* instructionTexture;
    SDL_Texture* instructionBgTexture;
    SDL_Texture* gameOverTexture;
    SDL_Texture* victoryTexture;
    SDL_Texture* playerTankTexture;
    SDL_Rect playerTankRect;

    Audio audio;
    bool loadAudioResources();

    enum class GameState
    {
        MENU,
        INSTRUCTIONS,
        PLAYING,
        GAME_OVER,
        VICTORY,
        CONSTRUCTION
};
    GameState state;
     struct MenuButton {
        SDL_Rect rect;
        SDL_Texture* texture;
        bool isHovered;
    };

    void updateConstruction();

    void handleConstructionEvents();
    void renderConstruction();

    MenuButton player1Button;
    MenuButton constructionButton;
    int selectedMenuOption; // 0 = 1 PLAYER, 1 = CONSTRUCTION
    TTF_Font* menuFont;
    SDL_Texture* menuBackground;
    SDL_Texture* createButtonTexture(const std::string& text);

    struct Button
    {
        SDL_Rect rect;
        SDL_Texture* texture;
        SDL_Texture* hoverTexture;
        bool isHovered = false;
    };

    Button playButtonMenu;
    Button playButtonGame;
    Button retryButton;
    Button menuButton;

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

    void renderText(const std::string& text, int x, int y, SDL_Color color);

};
