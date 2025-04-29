#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "PlayerTank.h"
#include "Wall.h"
#include "EnemyTank.h"
#include "Constants.h"

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    PlayerTank player;
    std::vector<Wall> walls;
    std::vector<EnemyTank> enemies;
    int enemyNumber = 3;
    bool running;

    enum class GameState {
        MAIN_MENU,
        INSTRUCTIONS,
        PLAYING,
        GAME_OVER,
        VICTORY,
        CONSTRUCTION
    };
    GameState state;

    struct Button {
        SDL_Rect rect;
        SDL_Texture* normal;
        SDL_Texture* hover;
        float scale = 1.0f;
        float targetScale = 1.0f;
        bool isHovered = false;

        void update() {
            const float speed = 0.1f;
            scale += (targetScale - scale) * speed;
        }
        bool contains(int x, int y) const {
        return (x >= rect.x && x <= rect.x + rect.w &&
                y >= rect.y && y <= rect.y + rect.h);
    }
    };
    Game();
    ~Game();

    void run();
    void handleEvents();
    void update(float deltaTime = 0.0f);  // Updated
    void render();
    void resetGame();
    void updateMusic();
    // Game objects
    TTF_Font* font;
    TTF_Font* boldfont;
    Button playButton;
    Button helpButton;
    Button retryButton;    // Added
    Button menuButton;     // Added

    SDL_Texture* backgroundTexture;
    SDL_Texture* tankGuideTexture;
    SDL_Texture* gameOverTexture;
    SDL_Texture* victoryTexture;
    SDL_Texture* instructionTexture;

    // Audio
    Mix_Music* bgMusic;
    Mix_Chunk* clickSound;
    Mix_Chunk* hoverSound;
    Mix_Chunk* bulletSound;
    Mix_Chunk* gameOverSound;
    Mix_Chunk* victorySound;


    // Helper methods
    void initMenu();
    void generateWalls();
    void spawnEnemies();
    SDL_Texture* loadTexture(const std::string& path);
    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void renderBoldText(const std::string& text, int x, int y, SDL_Color color);
    bool isMouseOver(const SDL_Rect& rect, int x, int y);
    void updateTankAnimation();
    void renderTank();

    // State handlers
    void handleMenuEvents();
    void handleInstructionEvents();
    void handleGameOverEvents();
    void handleVictoryEvents();
    void renderMenu();
    void renderInstructions();
    void renderGameOver();
    void renderVictory();
};
