#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <map>
#include "PlayerTank.h"
#include "Wall.h"
#include "EnemyTank.h"
#include "Constants.h"
#include "ScoreManager.h"

class Game {
public:
    Game();
    ~Game();

    void run();
    // Game state enum
    enum class GameState {
        MAIN_MENU,
        INSTRUCTIONS,
        PLAYING,
        GAME_OVER,
        VICTORY,
        GAME_COMPLETE,
        CONSTRUCTION
    };

    // Button struct
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

    // SDL components
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Game objects
    PlayerTank player;
    std::vector<Wall> walls;
    std::vector<EnemyTank> enemies;

    // UI elements
    TTF_Font* font = nullptr;
    TTF_Font* smallfont = nullptr;
    TTF_Font* boldfont = nullptr;

    Button playButton;
    Button helpButton;
    Button retryButton;
    Button menuButton;
    Button nextLevelButton;

    // Textures
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* tankGuideTexture = nullptr;
    SDL_Texture* gameOverTexture = nullptr;
    SDL_Texture* victoryTexture = nullptr;
    SDL_Texture* gameCompleteTexture = nullptr;
    SDL_Texture* instructionTexture = nullptr;
    SDL_Texture* wallTexture = nullptr;
    SDL_Texture* enemyTexture = nullptr;

    // Audio
    Mix_Music* bgMusic = nullptr;
    Mix_Chunk* clickSound = nullptr;
    Mix_Chunk* hoverSound = nullptr;
    Mix_Chunk* bulletSound = nullptr;
    Mix_Chunk* gameOverSound = nullptr;
    Mix_Chunk* victorySound = nullptr;
    Mix_Chunk* explodeSound = nullptr;

    // Game state
    bool running = true;
    GameState state = GameState::MAIN_MENU;
    GameState targetState = GameState::MAIN_MENU;
    bool transitioning = false;
    float fadeAlpha = 0.0f;
    bool fadeIn = false;

    // Game progression
    int currentLevel = 1;
    const int MAX_LEVEL = 3;
    ScoreManager scoreManager;
    int score = 0;
    std::map<int, int> highScores;
    int enemyNumber = 0;


    // Input
    int mouseX = 0;
    int mouseY = 0;

    // Core game methods
    void handleEvents();
    void update(float deltaTime = 0.0f);
    void render();
    void resetGame();
    void resetPlayer();
    void updateMusic();

    // Helper methods
    void initMenu();
    void generateWalls();
    void spawnEnemies();
    SDL_Texture*loadTexture(const char* path);
    static SDL_Texture* LoadTexture(SDL_Renderer* renderer, const std::string& filePath);
    void renderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font = nullptr);
    void renderBoldText(const std::string& text, int x, int y, SDL_Color color);

    // State handlers
    void handleMenuEvents();
    void handleInstructionEvents();
    void handleGameOverEvents();
    void handleVictoryEvents();
    void handleGameCompleteEvents();
    void renderMenu();
    void renderInstructions();
    void renderGameOver();
    void renderVictory();
    void renderGameComplete();

    // Transition methods
    void startTransition(GameState newState);
    void updateTransition(float deltaTime);
    void renderTransition();
    // Level management
    void loadLevel(int levelNumber);
    void generateDefaultLevel(int levelNumber);
    void spawnEnemy(int levelNumber);
    void startLevelTransition(int newLevel);
    void enemyKilled();
    void loadHighScores();
    void saveHighScores();

};
