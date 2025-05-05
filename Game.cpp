#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <algorithm>
#include<fstream>

Game::Game():player(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }

    // Create window
    window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }

    // Initialize SDL extensions
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cerr << "SDL_image init failed: " << IMG_GetError() << std::endl;
    }
    if (!player.loadTexture(renderer, "D:/A_Teaching/LTNC/2024/DEMO/playertank1.png")) {
        std::cout << "Failed to load player tank texture!" << std::endl;
    }
     enemyTexture = IMG_LoadTexture(renderer, "D:/A_Teaching/LTNC/2024/DEMO/enemies.png");
    if (!enemyTexture) {
        std::cout << "Failed to load enemy texture: " << SDL_GetError() << std::endl;
    }
    wallTexture = IMG_LoadTexture(renderer, "D:/A_Teaching/LTNC/2024/DEMO/wall.png");
    if (!wallTexture) {
        std::cout << "Failed to load wall texture: " << SDL_GetError() << std::endl;
    }

    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    // Load font
    font = TTF_OpenFont("D:/A_Teaching/LTNC/2024/DEMO/Core Narae Pro W01 Pro.ttf", 35);
    smallfont = TTF_OpenFont("D:/A_Teaching/LTNC/2024/DEMO/Cabin-VariableFont_wdth,wght.ttf",20);
    boldfont = TTF_OpenFont("D:/A_Teaching/LTNC/2024/DEMO/Core Narae Pro W01 Pro Bold.ttf", 35);
    if (!font || !smallfont) {
        std::cerr << "Failed to load font! Using fallback." << std::endl;
    }
    if (!boldfont) {
        std::cerr << "Failed to load bold font! Using fallback." << std::endl;
    }

    initMenu();

    running = true;
    transitioning = false;
    fadeIn = false;
    fadeAlpha = 0.0f;
    targetState = GameState::MAIN_MENU;
    currentLevel = 1;
    score = 0;
    loadHighScores();
    loadLevel(1);
    SDL_Log("Game initialization complete, state: %d, targetState: %d",
            static_cast<int>(state), static_cast<int>(targetState));

}

Game::~Game() {
    // Free textures
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(tankGuideTexture);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(victoryTexture);
    SDL_DestroyTexture(instructionTexture);
    SDL_DestroyTexture(enemyTexture);
    // Free buttons textures
    SDL_DestroyTexture(playButton.normal);
    SDL_DestroyTexture(playButton.hover);
    SDL_DestroyTexture(helpButton.normal);
    SDL_DestroyTexture(helpButton.hover);
    SDL_DestroyTexture(retryButton.normal);
    SDL_DestroyTexture(retryButton.hover);
    SDL_DestroyTexture(menuButton.normal);
    SDL_DestroyTexture(menuButton.hover);
    SDL_DestroyTexture(gameCompleteTexture);

    Mix_FreeMusic(bgMusic);
    Mix_FreeChunk(clickSound);
    Mix_FreeChunk(hoverSound);
    Mix_FreeChunk(bulletSound);
    Mix_FreeChunk(gameOverSound);
    Mix_FreeChunk(victorySound);
    Mix_FreeChunk(explodeSound);

    // Close font
    TTF_CloseFont(font);
    TTF_CloseFont(smallfont);
    TTF_CloseFont(boldfont);

    // Quit subsystems
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
void Game::run() {
    Uint32 lastTime = SDL_GetTicks();
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (static_cast<int>(state) < static_cast<int>(GameState::MAIN_MENU) ||
            static_cast<int>(state) > static_cast<int>(GameState::GAME_COMPLETE)) {
            SDL_Log("LỖI: State không hợp lệ: %d, đặt lại thành MAIN_MENU", static_cast<int>(state));
            state = GameState::MAIN_MENU;
        }

        if (transitioning) {
            updateTransition(deltaTime);
        }

        // Xử lý sự kiện và render theo trạng thái hiện tại
        switch (state) {
            case GameState::MAIN_MENU:
                handleMenuEvents();
                renderMenu();
                break;
            case GameState::INSTRUCTIONS:
                handleInstructionEvents();
                renderInstructions();
                break;
            case GameState::PLAYING:
                handleEvents();
                update(deltaTime);
                render();
                break;
            case GameState::GAME_OVER:
                handleGameOverEvents();
                renderGameOver();
                break;
            case GameState::VICTORY:
                handleVictoryEvents();
                renderVictory();
                break;
            case GameState::GAME_COMPLETE:
                handleGameCompleteEvents();
                renderGameComplete();
            break;
            default:
                SDL_Log("Unknown game state: %d", static_cast<int>(state));
                running = false;
                break;
        }
        renderTransition();
        Uint32 frameTime = SDL_GetTicks() - currentTime;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
    }
}

void Game::renderBoldText(const std::string& text, int x, int y, SDL_Color color) {
    // Vẽ chữ nhiều lần với offset nhỏ để tạo hiệu ứng đậm
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 || j != 0) {
                renderText(text, x + i, y + j, color);
            }
        }
    }
    // Vẽ lần cuối ở vị trí chính xác
    renderText(text, x, y, color);
}

SDL_Texture* Game::loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "Failed to load surface from " << path << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "Failed to create texture from " << path << ": " << SDL_GetError() << std::endl;
    }
    return texture;
}

void Game::initMenu() {
    // Load menu textures
    backgroundTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/bg.jpg");
    tankGuideTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/Guide.jpg");
    gameOverTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/loser.jpg");
    victoryTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/winnerlevel.jpg");
    instructionTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/Guide.jpg");
    gameCompleteTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/winner.jpg");

    // Initialize buttons
    playButton.normal = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button.jpg");
    playButton.hover = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button.jpg");
    playButton.rect = {300, 250, 200, 80};

    helpButton.normal = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button1.jpg");
    helpButton.hover = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button1.jpg");
    helpButton.rect = {300, 350, 200, 80};

    retryButton.normal = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button1.jpg");
    retryButton.hover = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button1.jpg");
    retryButton.rect = {0, 0, 200, 80}; // Position set in renderGameOver()

    menuButton.normal = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button.jpg");
    menuButton.hover = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button.jpg");
    menuButton.rect = {0, 0, 200, 80}; // Position set in render functions

    nextLevelButton.normal = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button1.jpg");
    nextLevelButton.hover = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button1.jpg");
    nextLevelButton.rect = {300, 300, 200, 80};
    // Load audio
    bgMusic = Mix_LoadMUS("D:/A_Teaching/LTNC/2024/DEMO/opening.mp3");
    clickSound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/mouseclick.mp3");
    hoverSound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/mouseclick.mp3");
    explodeSound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/explore.mp3");
    bulletSound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/bum.mp3");
    gameOverSound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/loser.mp3");
    victorySound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/winner.mp3");
    // Set audio volumes
    Mix_VolumeMusic(64); // 50% volume
    Mix_Volume(-1, 128); // 100% volume for sound effects

    // Play background music
    Mix_PlayMusic(bgMusic, -1); // Loop indefinitely
}

void Game::generateWalls() {
   for(int i = 3; i < MAP_HEIGHT - 3; i += 2) {
    for(int j = 3; j < MAP_WIDTH - 3; j += 2) {
        walls.emplace_back(j * TILE_SIZE, i * TILE_SIZE,wallTexture);
    }
}

}
void Game::startLevelTransition(int newLevel) {
    if (transitioning) return;
    currentLevel = newLevel;
    loadLevel(currentLevel);
    startTransition(GameState::PLAYING);
}
void Game::spawnEnemies() {
    enemies.clear();
    for (int i = 0; i < initialEnemyCount; ++i) {
        int ex, ey;
        bool valid = false;
        int maxAttempts = 100; // Giới hạn số lần thử

        while (!valid && maxAttempts > 0) {
            ex = (rand() % (MAP_WIDTH-2)+1) * TILE_SIZE;
            ey = (rand() % (MAP_HEIGHT-2)+1) * TILE_SIZE;
            valid = true;

            for (const auto& wall : walls) {
                if (wall.active && wall.x == ex && wall.y == ey) {
                    valid = false;
                    break;
                }
            }
            maxAttempts--;
        }

        if (valid) {
            EnemyTank e(ex, ey);
            e.setTexture(enemyTexture);
            e.setSpeed(2.0f);
            enemies.push_back(e);
        } else {
            std::cerr << "Could not find valid position for enemy " << i << std::endl;
        }
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    player.move(0, -5, walls);
                    break;
                case SDLK_DOWN:
                    player.move(0, 5, walls);
                    break;
                case SDLK_LEFT:
                    player.move(-5, 0, walls);
                    break;
                case SDLK_RIGHT:
                    player.move(5, 0, walls);
                    break;
                case SDLK_SPACE:
                    player.shoot();
                    if (bulletSound) {
                        Mix_PlayChannel(-1, bulletSound, 0);
                    }
                    break;
            }
        }
    }
}

void Game::update(float deltaTime) {
    if (isTimerRunning && state == GameState::PLAYING) {
        currentPlayTime = SDL_GetTicks() - levelStartTime;
    }
    updateTransition(deltaTime);
    if (transitioning || state != GameState::PLAYING) return;
    updateMusic();
    player.updateBullets();

    for (auto& bullet : player.bullets) {
        for (auto& enemy : enemies) {
            if (enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)) {
                enemy.active = false;
                bullet.active = false;
                Mix_PlayChannel(-1, explodeSound, 0);
                enemyKilled();
                break;
            }
        }
    }

    for (auto& enemy : enemies) {
        enemy.move(walls);
        enemy.updateBullets();
        if (rand() % 100 < 2) {
            enemy.shoot();
        }
    }

    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            for (auto& wall : walls) {
                if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                    wall.active = false;
                    bullet.active = false;
                    break;
                }
            }
        }
    }
    for (auto& bullet : player.bullets) {
        for (auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                wall.active = false;
                bullet.active = false;
                break;
            }
        }
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](EnemyTank &e) { return !e.active; }), enemies.end());

    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            if (SDL_HasIntersection(&bullet.rect, &player.rect)) {
                Mix_PlayChannel(-1, gameOverSound, 0);
                startTransition(GameState::GAME_OVER);
                return;
            }
        }
    }
}

void Game::updateMusic() {
    if ((state == GameState::MAIN_MENU || state == GameState::INSTRUCTIONS)) {
        if (!Mix_PlayingMusic()) {
            Mix_PlayMusic(bgMusic, -1);
        }
    } else {
        if (Mix_PlayingMusic()) {
            Mix_HaltMusic();
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 197,198,227, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,252,241,234, 255);

    for(int i = 1; i< MAP_HEIGHT - 1; i++)
        {
            for(int j = 1; j< MAP_WIDTH - 1; j++)
            {
                SDL_Rect tile = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                SDL_RenderFillRect(renderer,&tile);
            }
        }
        for(size_t i=0;i<walls.size();i++)
        {   for(size_t i=0;i< walls.size();i++)
            {
                walls[i].render(renderer);
            }
        }

        player.render(renderer);
        for(auto &enemy : enemies)
        {
            enemy.render(renderer);
        }

        renderText(std::string("Level: ") + std::to_string(currentLevel), 6, 9, {45, 87, 67, 255}, smallfont);
        renderText(std::string("Score: ") + std::to_string(score), 6, 39, {45, 87, 67, 255}, smallfont);
        renderText(std::string("High Score: ") + std::to_string(highScores[currentLevel]), 6, 69, {45, 87, 67, 255}, smallfont);

        if (comboCount > 1 && (SDL_GetTicks() - lastKillTime) <= COMBO_TIME_WINDOW) {
            renderText(std::string("COMBO x") + std::to_string(comboCount), 650, 10, {244, 127, 135 ,255},smallfont);
        }

        if (state == GameState::PLAYING) {
            Uint32 seconds = currentPlayTime / 1000;
            Uint32 minutes = seconds / 60;
            seconds %= 60;
            std::string timeText = std::string("TIME: ") +
                               (minutes < 10 ? "0" : "") +
                               std::to_string(minutes) +
                               ":" +
                               (seconds < 10 ? "0" : "") +
                               std::to_string(seconds);
            renderText(timeText, 6, 99, {45, 87, 67, 255}, smallfont);
        }
        renderTransition();
        SDL_RenderPresent(renderer);
}


void Game::renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }
    playButton.rect = {150, 250, 200, 150};
    playButton.update();
    helpButton.rect = {150, 350, 190, 100};
    helpButton.update();
    SDL_RenderCopy(renderer, playButton.normal, nullptr, &playButton.rect);
    SDL_RenderCopy(renderer, helpButton.normal, nullptr, &helpButton.rect);

    if (font && boldfont) {
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        renderText("PLAY", 300, 310,{167,112,79, 255},font);
        renderText("CONSTRUCTION",300, 380,{167,112,79, 255},font);
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }
    SDL_RenderPresent(renderer);
}
void Game::handleMenuEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        bool wasHoveringPlay = playButton.isHovered;
        bool wasHoveringHelp = helpButton.isHovered;

        playButton.isHovered = isMouseOver(playButton.rect, mouseX, mouseY);
        helpButton.isHovered = isMouseOver(helpButton.rect, mouseX, mouseY);

        if ((playButton.isHovered && !wasHoveringPlay) ||
            (helpButton.isHovered && !wasHoveringHelp)) {
            Mix_PlayChannel(-1, hoverSound, 0);
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (playButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                startTransition(GameState::PLAYING);

            } else if (helpButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                startTransition(GameState::INSTRUCTIONS);
            }
        }
    }
    playButton.targetScale = playButton.isHovered ? 1.05f : 1.0f;
    helpButton.targetScale = helpButton.isHovered ? 1.05f : 1.0f;
}

void Game::renderInstructions() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    if (instructionTexture) {
        SDL_RenderCopy(renderer, instructionTexture, nullptr, nullptr);
    }
     if (font && boldfont)
    {
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        renderText("BACK", 350, 480,{167,112,79, 255}, font);
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }
    menuButton.rect = {300, 460, 200, 200};
    SDL_RenderCopy(renderer,
                  menuButton.isHovered ? menuButton.hover : menuButton.normal,
                  nullptr, &menuButton.rect);

    SDL_RenderPresent(renderer);
}

void Game::handleInstructionEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        menuButton.isHovered = isMouseOver(menuButton.rect, mouseX, mouseY);
        menuButton.targetScale = menuButton.isHovered ? 1.05f : 1.0f;

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (menuButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                startTransition(GameState::MAIN_MENU);
            }
        }
    }
}


void Game::renderGameOver() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (gameOverTexture)
    {
        SDL_RenderCopy(renderer, gameOverTexture, nullptr, nullptr);
    }

    retryButton.rect = {470, 400, 150, 150};
    menuButton.rect = {180, 380, 200, 200};

    if (font && boldfont)
    {
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        renderText("MENU", 230, 390,{167,112,79, 255},font);
        renderText("PLAY", 500, 390,{167,112,79, 255},font);
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }
    SDL_RenderCopy(renderer,
                  retryButton.isHovered ? retryButton.hover : retryButton.normal,
                  nullptr, &retryButton.rect);
    SDL_RenderCopy(renderer,
                  menuButton.isHovered ? menuButton.hover : menuButton.normal,
                  nullptr, &menuButton.rect);

    SDL_RenderPresent(renderer);
}
void Game::resetPlayer() {
    player = PlayerTank(((MAP_WIDTH - 1) / 2) * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE);

    SDL_Texture* tex = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/playertank1.png");
    if (!tex) {
        std::cout << "Failed to load player texture!" << std::endl;
    } else {
        std::cout << "Loaded player texture successfully." << std::endl;
        player.setTexture(tex);
    }

}

void Game::handleGameOverEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
        if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            retryButton.isHovered = isMouseOver(retryButton.rect, mouseX, mouseY);
            menuButton.isHovered = isMouseOver(menuButton.rect, mouseX, mouseY);
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (retryButton.isHovered) {
                    resetGame();
                    loadLevel(currentLevel);
                    Mix_PlayChannel(-1, clickSound, 0);
                    startTransition(GameState::PLAYING);
                }
                else if (menuButton.isHovered) {
                    resetGame();
                    loadLevel(1);
                    Mix_PlayChannel(-1, clickSound, 0);
                    startTransition(GameState::MAIN_MENU);
                }
            }
        }
    }
    retryButton.targetScale = retryButton.isHovered ? 1.05f : 1.0f;
    menuButton.targetScale = menuButton.isHovered ? 1.05f : 1.0f;
}

void Game::renderVictory() {
    std::cout << "Rendering Victory screen - currentLevel: " << currentLevel << ", MAX_LEVEL: " << MAX_LEVEL << std::endl;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (victoryTexture) {
        SDL_RenderCopy(renderer, victoryTexture, nullptr, nullptr);
    }

    if (font && boldfont) {
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);

        // Hiển thị thông tin level
        renderText("Level Complete!", 230, 220, {167, 112, 79, 255}, font);
        renderText(std::string("Score: ") + std::to_string(score), 230, 270, {167, 112, 79, 255}, font);
        renderText(std::string("High Score: ") + std::to_string(highScores[currentLevel]), 230, 320, {167, 112, 79, 255}, font);

        // Hiển thị thời gian
        Uint32 seconds = currentPlayTime / 1000;
        Uint32 minutes = seconds / 60;
        seconds %= 60;
        std::string timeText = std::string("TIME: ") +
                             (minutes < 10 ? "0" : "") +
                             std::to_string(minutes) +
                             ":" +
                             (seconds < 10 ? "0" : "") +
                             std::to_string(seconds);
        renderText(timeText, 230, 370, {167, 112, 79, 255}, font);

        // LUÔN hiển thị cả 2 nút
        renderText("Menu", 200, 420, {167, 112, 79, 255}, font);
        renderText("Next Level", 440, 420, {167, 112, 79, 255}, font);

        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }

    // Đặt vị trí và render các nút
    menuButton.rect = {130, 410, 250, 200};
    nextLevelButton.rect = {420, 430, 200, 150};

    SDL_RenderCopy(renderer,
                 menuButton.isHovered ? menuButton.hover : menuButton.normal,
                 nullptr, &menuButton.rect);

    SDL_RenderCopy(renderer,
                 nextLevelButton.isHovered ? nextLevelButton.hover : nextLevelButton.normal,
                 nullptr, &nextLevelButton.rect);

    SDL_RenderPresent(renderer);
}

void Game::handleVictoryEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Luôn kiểm tra cả 2 nút
        menuButton.isHovered = isMouseOver(menuButton.rect, mouseX, mouseY);
        nextLevelButton.isHovered = isMouseOver(nextLevelButton.rect, mouseX, mouseY);

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (menuButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                startTransition(GameState::MAIN_MENU);
            }
            else if (nextLevelButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);

                if (currentLevel < MAX_LEVEL) {
                    currentLevel++;
                    resetGame();
                    loadLevel(currentLevel);
                    startTransition(GameState::PLAYING);
                } else {
                    // Nếu là level cuối, chuyển sang màn hình hoàn thành game
                    startTransition(GameState::GAME_COMPLETE);
                }
            }
        }
    }
}
void Game::renderGameComplete() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (gameCompleteTexture) {
         SDL_RenderCopy(renderer, gameCompleteTexture, nullptr, nullptr);
    }
    if (font && boldfont)
    {
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        renderText("MENU", 350, 400,{167,112,79, 255},font);
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }
    menuButton.rect = {280, 410, 250, 200};
    SDL_RenderCopy(renderer,
                  menuButton.isHovered ? menuButton.hover : menuButton.normal,
                  nullptr, &menuButton.rect);


    SDL_RenderPresent(renderer);
}
void Game::resetGame() {
    player = PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE, (MAP_HEIGHT-2)*TILE_SIZE);
    resetPlayer();
    walls.clear();
    enemies.clear();
    player.bullets.clear();
    isTimerRunning = false;
    currentPlayTime = 0;
    score = 0;
    comboCount = 0;
    lastKillTime = 0;
}
void Game::handleGameCompleteEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        menuButton.rect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT - 150, 200, 80};
        menuButton.isHovered = isMouseOver(menuButton.rect, mouseX, mouseY);

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (menuButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                startTransition(GameState::MAIN_MENU);
            }
        }
    }
}
bool Game::isMouseOver(const SDL_Rect& rect, int x, int y) {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

void Game::renderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font) {
    if (!font) {
        if (!smallfont) return;
        font = smallfont;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        SDL_Log("TTF_RenderText_Solid failed: %s", TTF_GetError());
        return;
    }
    int w = surface->w;
    int h = surface->h;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return;
    }

    SDL_Rect rect = {x, y, w, h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    SDL_DestroyTexture(texture);
}


void Game::startTransition(GameState newState) {
    if (transitioning) {
        std::cout << "Transition already in progress" << std::endl;
        return;
    }
    if (newState < GameState::MAIN_MENU || newState > GameState::GAME_COMPLETE) {

        std::cerr << "Invalid game state requested: " << static_cast<int>(newState) << std::endl;
        return;
    }

    targetState = newState;
    transitioning = true;
    fadeIn = true;
    fadeAlpha = 0.0f;
}

void Game::updateTransition(float deltaTime) {
    if (!transitioning) return;
    const float fadeSpeed = 500.0f * deltaTime;
    if (fadeIn) {
        fadeAlpha += fadeSpeed;
        if (fadeAlpha >= 255.0f) {
            fadeAlpha = 255.0f;
            state = targetState;
            fadeIn = false;
            if (state == GameState::PLAYING) {
                resetGame();
                loadLevel(currentLevel);
            }
            std::cout << "Transition complete. New state: " << static_cast<int>(state) << std::endl;
        }
    } else {
        fadeAlpha -= fadeSpeed;
        if (fadeAlpha <= 0.0f) {
            fadeAlpha = 0.0f;
            transitioning = false;
            std::cout << "Fade out complete. Transitioning: " << transitioning << std::endl;
        }
    }
}

void Game::renderTransition() {
    if (!transitioning && fadeAlpha <= 0.0f) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(fadeAlpha));
    SDL_Rect fullScreen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &fullScreen);
}

void Game::loadLevel(int levelNumber) {
    // Kiểm tra hợp lệ levelNumber
    if (levelNumber < 1 || levelNumber > MAX_LEVEL) {
        SDL_Log("Invalid level number: %d (Valid range: 1-%d)", levelNumber, MAX_LEVEL);
        if (levelNumber > MAX_LEVEL) {
            startTransition(GameState::GAME_COMPLETE);
            return;
        }
        levelNumber = 1; // Fallback về level 1 nếu không hợp lệ
    }

    // Reset game state
    currentLevel = levelNumber;
    walls.clear();
    enemies.clear();
    player.bullets.clear();
    comboCount = 0;
    lastKillTime = 0;
    score = 0;
    isTimerRunning = true;
    levelStartTime = SDL_GetTicks();

    // Load level data
    std::string path = "D:/A_Teaching/LTNC/2024/DEMO/level" + std::to_string(levelNumber) + ".txt";
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Cannot open level file: " << path << " - Generating default level\n";
        generateDefaultLevel(levelNumber);
        SDL_Log("Generated default level %d", currentLevel);
        return;
    }

    // Parse level file
    std::string line;
    int y = 0;
    while (std::getline(file, line)) {
        for (int x = 0; x < line.size() && x < MAP_WIDTH; x++) {
            char c = line[x];
            switch (c) {
                case '#':
                    if (wallTexture) {
                        walls.emplace_back(x * TILE_SIZE, y * TILE_SIZE, wallTexture);
                    }
                    break;
                case 'E':
                    if (enemyTexture) {
                        EnemyTank enemy(x * TILE_SIZE, y * TILE_SIZE);
                        enemy.setTexture(enemyTexture);
                        enemy.setSpeed(1.0f + (levelNumber * 0.5f));
                        enemies.push_back(enemy);
                    }
                    break;
                case 'P':
                    player.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                    break;
                case '.':
                    break;
                default:
                    SDL_Log("Unknown character '%c' at (%d,%d)", c, x, y);
            }
        }
        if (++y >= MAP_HEIGHT) break;
    }
    file.close();

    // Cập nhật trạng thái enemy
    initialEnemyCount = enemies.size();
    remainingEnemies = initialEnemyCount;

    SDL_Log("Level %d loaded: %d walls, %d enemies",
           currentLevel, walls.size(), enemies.size());
}

void Game::generateDefaultLevel(int levelNumber) {
    walls.clear();
    enemies.clear();
    int wallSpacing = 2 + (levelNumber % 3);
    for (int i = 3; i < MAP_HEIGHT - 3; i += wallSpacing) {
        for (int j = 3; j < MAP_WIDTH - 3; j += wallSpacing) {
            if (rand() % 2 == 0) {
                walls.emplace_back(j * TILE_SIZE, i * TILE_SIZE, wallTexture);
            }
        }
    }
    int enemyCount = 3 + levelNumber * 2;
    for (int i = 0; i < enemyCount; ++i) {
        spawnEnemy(levelNumber);
    }
    player.setPosition(((MAP_WIDTH-1)/2)*TILE_SIZE, (MAP_HEIGHT-2)*TILE_SIZE);
    std::cout << "Generated default level " << levelNumber << " with " << walls.size() << " walls and " << enemies.size() << " enemies\n";
}
void Game::enemyKilled() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastKillTime > COMBO_TIME_WINDOW) {
        comboCount = 0;
    }

    comboCount++;
    lastKillTime = currentTime;
    float multiplier = 1.0f;
    for (const auto& [kills, bonus] : COMBO_MULTIPLIERS) {
        if (comboCount >= kills) {
            multiplier = bonus;
        }
    }
    int basePoints = 100;
    score += static_cast<int>(basePoints * multiplier);
    remainingEnemies--;
    if (remainingEnemies <= 0) {
        Uint32 levelTime = (SDL_GetTicks() - levelStartTime) / 1000;
        Uint32 targetTime = 60 + (currentLevel * 30);
        if (levelTime < targetTime) {
            score += (targetTime - levelTime) * TIME_BONUS_PER_SECOND;
        }
        updateHighScores();
        startTransition(GameState::VICTORY);
    }
}
void Game::updateHighScores() {
    if (score > highScores[currentLevel]) {
        highScores[currentLevel] = score;
        saveHighScores();
    }
}

void Game::loadHighScores() {
    scoreManager.loadHighScores(highScores);
}

void Game::saveHighScores() {
    scoreManager.saveHighScores(highScores);
}

void Game::spawnEnemy(int levelNumber) {
    int ex, ey;
    bool valid = false;

    while (!valid) {
        ex = (rand() % (MAP_WIDTH-2) + 1) * TILE_SIZE;
        ey = (rand() % (MAP_HEIGHT-2) + 1) * TILE_SIZE;
        valid = true;

        for (const auto& wall : walls) {
            if (wall.active && wall.x == ex && wall.y == ey) {
                valid = false;
                break;
            }
        }
        if (abs(ex - player.x) < 2*TILE_SIZE && abs(ey - player.y) < 2*TILE_SIZE) {
            valid = false;
        }
    }

    EnemyTank e(ex, ey);
    e.setTexture(enemyTexture);
    e.setSpeed(1.0f + (levelNumber * 0.5f));
    enemies.push_back(e);
}
void Game::completeLevel() {
    isTimerRunning = false;
    Uint32 levelTime = (SDL_GetTicks() - levelStartTime) / 1000;
    Uint32 targetTime = 120;
    if (levelTime < targetTime) {
        int timeBonus = (targetTime - levelTime) * TIME_BONUS_PER_SECOND;
        score += timeBonus;
    }
    updateHighScores();
    startTransition(GameState::VICTORY);
}



