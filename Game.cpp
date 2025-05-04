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
    boldfont = TTF_OpenFont("D:/A_Teaching/LTNC/2024/DEMO/Core Narae Pro W01 Pro Bold.ttf", 35);
    if (!font) {
        std::cerr << "Failed to load font! Using fallback." << std::endl;
    }
    if (!boldfont) {
        std::cerr << "Failed to load bold font! Using fallback." << std::endl;
    }

    // Initialize game objects
    //generateWalls();
    //spawnEnemies();
    initMenu();

    // Set initial state
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

    Mix_FreeMusic(bgMusic);
    Mix_FreeChunk(clickSound);
    Mix_FreeChunk(hoverSound);
    Mix_FreeChunk(bulletSound);
    Mix_FreeChunk(gameOverSound);
    Mix_FreeChunk(victorySound);
    Mix_FreeChunk(explodeSound);

    // Close font
    TTF_CloseFont(font);
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
        while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (static_cast<int>(state) < static_cast<int>(GameState::MAIN_MENU) ||
            static_cast<int>(state) > static_cast<int>(GameState::CONSTRUCTION)) {
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
            case GameState::CONSTRUCTION:
                // Xử lý trạng thái construction
                break;
            default:
                SDL_Log("Unknown game state: %d", static_cast<int>(state));
                running = false;
                break;
        }

        // Render hiệu ứng chuyển cảnh (nếu có)
        renderTransition();

        // Giới hạn FPS
        Uint32 frameTime = SDL_GetTicks() - currentTime;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
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

SDL_Texture* Game::loadTexture(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cout << "Failed to load surface: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
    }
    return texture;
}

void Game::initMenu() {
    // Load menu textures
    backgroundTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/bg.jpg");
    tankGuideTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/Guide.jpg");
    gameOverTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/loser.jpg");
    victoryTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/winner.jpg");
    instructionTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/Guide.jpg");

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

    nextLevelButton.normal = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button.jpg");
    nextLevelButton.hover = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button.jpg");
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
    startTransition(GameState::PLAYING);  // Sửa thành PLAYING thay vì VICTORY
    loadLevel(currentLevel);  // Đảm bảo load level mới
}
void Game::spawnEnemies() {
    enemies.clear();
    for (int i = 0; i < enemyNumber; ++i) {
        int ex, ey;
        bool valid = false;

        while (!valid) {
            ex = (rand() % (MAP_WIDTH-2)+1) * TILE_SIZE;
            ey = (rand() % (MAP_HEIGHT-2)+1) * TILE_SIZE;
            valid = true;

            for (const auto& wall : walls) {
                if (wall.active && wall.x == ex && wall.y == ey) {
                    valid = false;
                    break;
                }
            }
        }

        EnemyTank e(ex, ey);
        e.setTexture(enemyTexture);
        e.setSpeed(2.0f);
        enemies.push_back(e);
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
                    Mix_PlayChannel(-1, bulletSound, 0);
                    break;
            }
        }
    }
}

void Game::update(float deltaTime) {
    updateTransition(deltaTime);
    if (transitioning || state != GameState::PLAYING) return;
    updateMusic();
    player.updateBullets();

    // Xử lý va chạm đạn và kẻ thù
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

    // Cập nhật kẻ thù
    for (auto& enemy : enemies) {
        enemy.move(walls);
        enemy.updateBullets();
        if (rand() % 100 < 2) {
            enemy.shoot();
        }
    }

    // Xử lý va chạm đạn và tường
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

    // Xóa kẻ thù không hoạt động
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](EnemyTank &e) { return !e.active; }), enemies.end());

    // Kiểm tra va chạm đạn kẻ thù với người chơi
    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            if (SDL_HasIntersection(&bullet.rect, &player.rect)) {
                Mix_PlayChannel(-1, gameOverSound, 0);
                startTransition(GameState::GAME_OVER);
                return;
            }
        }
    }

    // Kiểm tra điều kiện chiến thắng
    bool allEnemiesDead = true;
    for (const auto& enemy : enemies) {
        if (enemy.active) {
            allEnemiesDead = false;
            break;
        }
    }
    if (allEnemiesDead && !transitioning) {
        std::cout << "Victory condition met! Transitioning to VICTORY state." << std::endl;
        Mix_PlayChannel(-1, victorySound, 0);
        if (score > highScores[currentLevel]) {
            highScores[currentLevel] = score;
            saveHighScores();
        }
        startTransition(GameState::VICTORY);
    }
}

void Game::updateMusic() {
    if ((state == GameState::MAIN_MENU || state == GameState::INSTRUCTIONS)) {
        if (!Mix_PlayingMusic()) {
            Mix_PlayMusic(bgMusic, -1); // Phát nhạc nếu chưa chạy
        }
    } else {
        if (Mix_PlayingMusic()) {
            Mix_HaltMusic(); // Dừng nhạc nếu không phải main_menu hay instruction
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

        SDL_Color white = {255, 255, 255, 255};
        renderText("Level: " + std::to_string(currentLevel), 10, 10, white);
        renderText("Score: " + std::to_string(score), 10, 40, white);
        renderText("High Score: " + std::to_string(highScores[currentLevel]), 10, 70, white);
        renderTransition();
        SDL_RenderPresent(renderer);
    }


void Game::renderMenu() {
    // 1. Xóa màn hình
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }

    // 4. Đặt vị trí TỪNG NÚT theo cách dễ hiểu nhất
    // -----------------------------------------------
    // Nút Play - đặt tại x=300, y=300 (có thể thay đổi dễ dàng)
    playButton.rect = {150, 250, 200, 150}; // x, y, width, height
    playButton.update();

    // Nút Help - đặt dưới nút Play cách 100px
    helpButton.rect = {150, 350, 190, 100}; // x giống Play, y = y của Play + 100
    helpButton.update();
    // -----------------------------------------------

    // 5. Vẽ các nút
    SDL_RenderCopy(renderer, playButton.normal, nullptr, &playButton.rect);
    SDL_RenderCopy(renderer, helpButton.normal, nullptr, &helpButton.rect);

    if (font && boldfont) {
        // Chữ "PLAYER 1" bên phải nút Play
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        renderText("PLAY", 300, 310,{167,112,79, 255});

        // Chữ "CONSTRUCTION" bên phải nút Help
        renderText("CONSTRUCTION",300, 380,{167,112,79, 255});
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }

    // 7. Hiển thị
    SDL_RenderPresent(renderer);
}

void Game::handleMenuEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }

        // Get mouse state
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Check hover states
        bool wasHoveringPlay = playButton.isHovered;
        bool wasHoveringHelp = helpButton.isHovered;

        playButton.isHovered = isMouseOver(playButton.rect, mouseX, mouseY);
        helpButton.isHovered = isMouseOver(helpButton.rect, mouseX, mouseY);

        // Play hover sound if state changed
        if ((playButton.isHovered && !wasHoveringPlay) ||
            (helpButton.isHovered && !wasHoveringHelp)) {
            Mix_PlayChannel(-1, hoverSound, 0);
        }

        // Handle clicks
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (playButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                //state = GameState::PLAYING;
                startTransition(GameState::PLAYING);

            } else if (helpButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                //state = GameState::INSTRUCTIONS;
                startTransition(GameState::INSTRUCTIONS);
            }
        }
    }
    // Update button animations
    playButton.targetScale = playButton.isHovered ? 1.05f : 1.0f;
    helpButton.targetScale = helpButton.isHovered ? 1.05f : 1.0f;
}

void Game::renderInstructions() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render instruction background
    if (instructionTexture) {
        SDL_RenderCopy(renderer, instructionTexture, nullptr, nullptr);
    }
     if (font && boldfont)
    {
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        renderText("PLAY", 350, 480,{167,112,79, 255});
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }
    // Render back button
    menuButton.rect = {300, 450, 200, 200};
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
                //state = GameState::PLAYING;
                startTransition(GameState::PLAYING);
            }
        }
    }
}


void Game::renderGameOver() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render game over background
    if (gameOverTexture) {
        SDL_RenderCopy(renderer, gameOverTexture, nullptr, nullptr);
    }

    // Position buttons
    retryButton.rect = {470, 400, 150, 150};
    menuButton.rect = {180, 380, 200, 200};

    if (font && boldfont)
    {
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        renderText("MENU", 230, 390,{167,112,79, 255});
        renderText("PLAY", 500, 390,{167,112,79, 255});
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }
    // Render buttons
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
                    loadLevel(currentLevel); // Tải lại level hiện tại
                    Mix_PlayChannel(-1, clickSound, 0);
                    startTransition(GameState::PLAYING);
                }
                else if (menuButton.isHovered) {
                    resetGame();
                    loadLevel(1); // Tải level 1 khi quay lại menu
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render victory background
    if (victoryTexture) {
        SDL_RenderCopy(renderer, victoryTexture, nullptr, nullptr);
    }
    if (font && boldfont)
    {
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        //renderText("MENU", 350, 400,{167,112,79, 255});
        renderText("Level Complete!",300, 200, {167,112,79, 255});
        renderText("Score: " + std::to_string(score),300, 250,{167,112,79, 255});
        renderText("High Score: " + std::to_string(highScores[currentLevel]), 300, 300, {167,112,79, 255});
        renderText("Menu",280, 420, {167,112,79, 255});
        renderText("Next Level",480, 420,{167,112,79, 255});
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }
    // Position menu button
    menuButton.rect = {280, 410, 250, 200};
    nextLevelButton.rect = {400, 410, 250, 200};

    // Render button
    SDL_RenderCopy(renderer,
                  menuButton.isHovered ? menuButton.hover : menuButton.normal,
                  nullptr, &menuButton.rect);
    SDL_RenderCopy(renderer, nextLevelButton.normal, nullptr, &nextLevelButton.rect);
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

        menuButton.isHovered = isMouseOver(menuButton.rect, mouseX, mouseY);
        nextLevelButton.isHovered = isMouseOver(nextLevelButton.rect, mouseX, mouseY);

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (menuButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                startTransition(GameState::MAIN_MENU);
            }
            else if (nextLevelButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                int nextLevel = currentLevel + 1;
                if (nextLevel > MAX_LEVEL) {
                    nextLevel = 1; // Quay lại level 1 nếu đã hoàn thành tất cả
                }
                currentLevel = nextLevel;
                resetGame();
                loadLevel(currentLevel);
                startTransition(GameState::PLAYING);
            }
        }
    }
}

void Game::resetGame()
{
    player = PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE, (MAP_HEIGHT-2)*TILE_SIZE);
    resetPlayer();
    walls.clear();
    enemies.clear();
    player.bullets.clear();
    score = 0;
}

bool Game::isMouseOver(const SDL_Rect& rect, int x, int y) {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

void Game::renderText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect rect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}
void Game::startTransition(GameState newState) {
    if (transitioning) {
        std::cout << "Transition already in progress" << std::endl;
        return;
    }

    // Kiểm tra trạng thái hợp lệ
    if (newState < GameState::MAIN_MENU || newState > GameState::CONSTRUCTION) {
        std::cerr << "Invalid game state requested: " << static_cast<int>(newState) << std::endl;
        return;
    }

    targetState = newState;
    transitioning = true;
    fadeIn = true;
    fadeAlpha = 0.0f;

    // Debug log
    std::cout << "Starting transition to state: " << static_cast<int>(newState) << std::endl;
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
                loadLevel(currentLevel); // Tải level hiện tại
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
    walls.clear();
    enemies.clear();
    player.bullets.clear();

    // Reset player position
    player.setPosition(((MAP_WIDTH-1)/2)*TILE_SIZE, (MAP_HEIGHT-2)*TILE_SIZE);

    std::string path = "D:/A_Teaching/LTNC/2024/DEMO/level" + std::to_string(levelNumber) + ".txt";
    std::cout << "Attempting to load level from: " << path << std::endl;  // Debug

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << path << " - Error: " << std::endl;
        generateDefaultLevel(levelNumber);
        return;
    }

    std::string line;
    int y = 0;
    while (std::getline(file, line)) {
        std::cout << "Line " << y << ": " << line << std::endl;  // Debug
        for (int x = 0; x < line.size() && x < MAP_WIDTH; x++) {
            char c = line[x];
            switch (c) {
                case '#':
                    walls.emplace_back(x * TILE_SIZE, y * TILE_SIZE, wallTexture);
                    break;
                case 'E':
                    enemies.emplace_back(x * TILE_SIZE, y * TILE_SIZE);
                    enemies.back().setTexture(enemyTexture);
                    enemies.back().setSpeed(1.0f + (levelNumber * 0.5f));
                    break;
                case 'P':
                    player.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                    break;
                case '.':
                    // Empty space - do nothing
                    break;
                default:
                    std::cerr << "Unknown character in level file: '" << c << "' at ("
                              << x << "," << y << ")" << std::endl;
            }
        }
        y++;
        if (y >= MAP_HEIGHT) break;
    }
    file.close();

    // Debug info
    std::cout << "Loaded level " << levelNumber << " with:\n"
              << "- Walls: " << walls.size() << "\n"
              << "- Enemies: " << enemies.size() << "\n"
              << "- Player position: (" << player.x << "," << player.y << ")\n";

    if (enemies.empty()) {
        int enemyCount = 3 + levelNumber * 2;
        for (int i = 0; i < enemyCount; ++i) {
            spawnEnemy(levelNumber);
        }
    }
    score = 0; // Reset score for new level

}

void Game::generateDefaultLevel(int levelNumber) {
    walls.clear();
    enemies.clear();

    // Create walls with variation based on level
    int wallSpacing = 2 + (levelNumber % 3); // Vary spacing by level
    for (int i = 3; i < MAP_HEIGHT - 3; i += wallSpacing) {
        for (int j = 3; j < MAP_WIDTH - 3; j += wallSpacing) {
            if (rand() % 2 == 0) { // Randomly skip some walls
                walls.emplace_back(j * TILE_SIZE, i * TILE_SIZE, wallTexture);
            }
        }
    }

    // Spawn enemies based on level difficulty
    int enemyCount = 3 + levelNumber * 2; // More enemies in higher levels
    for (int i = 0; i < enemyCount; ++i) {
        spawnEnemy(levelNumber);
    }

    // Set player position
    player.setPosition(((MAP_WIDTH-1)/2)*TILE_SIZE, (MAP_HEIGHT-2)*TILE_SIZE);
    std::cout << "Generated default level " << levelNumber << " with " << walls.size() << " walls and " << enemies.size() << " enemies\n";
}
void Game::enemyKilled() {
    score += 100;
    // Có thể thêm hiệu ứng âm thanh/hình ảnh tại đây
}

void Game::loadHighScores() {
    std::ifstream in("highscores.txt");
    if (!in) return;
    int level, hs;
    while (in >> level >> hs) {
        if (level >= 1 && level <= MAX_LEVEL) {
            highScores[level] = hs;
        }
    }
    in.close();
}

void Game::saveHighScores() {
    std::ofstream out("highscores.txt");
    for (const auto& [level, hs] : highScores) {
        out << level << " " << hs << "\n";
    }
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
    e.setSpeed(1.0f + (levelNumber * 0.5f)); // Faster enemies in higher levels
    enemies.push_back(e);
}
