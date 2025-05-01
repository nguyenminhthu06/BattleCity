#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <algorithm>

Game::Game() : player(((MAP_WIDTH-1)/2)*TILE_SIZE, (MAP_HEIGHT-2)*TILE_SIZE,renderer) {
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
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
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
    generateWalls();
    spawnEnemies();
    initMenu();

    // Set initial state
    state = GameState::MAIN_MENU;
    running = true;
}

Game::~Game() {
    // Free textures
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(tankGuideTexture);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(victoryTexture);
    SDL_DestroyTexture(instructionTexture);

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

    // Close font
    TTF_CloseFont(font);

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
                // Handle construction state
                break;
            default:
                SDL_Log("Unknown game state!");
                break;
        }

        // Cap at ~60 FPS
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

    // Load audio
    bgMusic = Mix_LoadMUS("D:/A_Teaching/LTNC/2024/DEMO/opening.mp3");
    clickSound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/mouseclick.mp3");
    hoverSound = Mix_LoadWAV("assets/audio/hover.wav");
    bulletSound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/bum.mp3");
    gameOverSound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/loser.mp3");
    victorySound = Mix_LoadWAV("D:/A_Teaching/LTNC/2024/DEMO/winner.mp3");
    // Set audio volumes
    Mix_VolumeMusic(64); // 50% volume
    Mix_Volume(-1, 128); // 100% volume for sound effects

    // Play background music
    Mix_PlayMusic(bgMusic, -1); // Loop indefinitely
}

SDL_Texture* Game::loadTexture(const std::string &filePath) {
    SDL_Texture* texture = nullptr;
    texture = IMG_LoadTexture(renderer, filePath.c_str());

    if (texture == nullptr) {
        printf("Failed to load texture %s! SDL_image Error: %s\n",
               filePath.c_str(), IMG_GetError());
    }

    return texture;
}
void Game::generateWalls() {
   for(int i = 3; i < MAP_HEIGHT - 3; i += 2) {
    for(int j = 3; j < MAP_WIDTH - 3; j += 2) {
        walls.emplace_back(j * TILE_SIZE, i * TILE_SIZE);
    }
}

}

void Game::spawnEnemies() {
    enemies.clear();
    for(int i = 0; i < enemyNumber; i++) {
        int ex, ey;
        bool validPosition = false;

        while(!validPosition) {
            ex = (rand() % (MAP_WIDTH - 2) + 1) * TILE_SIZE;
            ey = (rand() % (MAP_HEIGHT - 2) + 1) * TILE_SIZE;
            validPosition = true;

            // Check collision with walls
            for(const auto& wall : walls) {
                if(wall.active && wall.x == ex && wall.y == ey) {
                    validPosition = false;
                    break;
                }
            }
        }
        enemies.emplace_back(ex, ey);
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
    // Update player bullets
    updateMusic();
    player.updateBullets();

    // Check player bullets hitting enemies
    for(auto& bullet : player.bullets) {
        for(auto& enemy : enemies) {
            if(enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)) {
                enemy.active = false;
                bullet.active = false;
                break;
            }
        }
    }

    // Update enemies
    for(auto& enemy : enemies) {
        enemy.move(walls);
        enemy.updateBullets();

        // Random enemy shooting
        if(rand() % 100 < 2) {
            enemy.shoot();
        }
    }

    // Check enemy bullets hitting walls
    for(auto& enemy : enemies) {
        for(auto& bullet : enemy.bullets) {
            for(auto& wall : walls) {
                if(wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                    wall.active = false;
                    bullet.active = false;
                    break;
                }
            }
        }
    }

    // Check player bullets hitting walls
    for(auto& bullet : player.bullets) {
        for(auto& wall : walls) {
            if(wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                wall.active = false;
                bullet.active = false;
                break;
            }
        }
    }

    // Remove inactive enemies
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](EnemyTank &e){ return !e.active; }), enemies.end());

    // Check victory condition
    if(enemies.empty()) {
        Mix_PlayChannel(-1, victorySound, 0);
        state = GameState::VICTORY;
    }

    // Check player hit by enemy bullets
    for(auto& enemy : enemies) {
        for(auto& bullet : enemy.bullets) {
            if(SDL_HasIntersection(&bullet.rect, &player.rect)) {
                Mix_PlayChannel(-1, victorySound, 0);
                state = GameState::GAME_OVER;
                return;
            }
        }
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
                state = GameState::PLAYING;
                Mix_HaltMusic();
            }
            else if (helpButton.isHovered) {
                Mix_PlayChannel(-1, clickSound, 0);
                state = GameState::INSTRUCTIONS;
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
        renderText("PLAY", 350, 460,{167,112,79, 255});
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
                state = GameState::MAIN_MENU;
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
    retryButton.rect = {490, 420, 200, 150};
    menuButton.rect = {180, 380, 200, 200};

    if (font && boldfont)
    {
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        renderText("PLAY", 230, 390,{167,112,79, 255});
        renderText("MENU", 520, 390,{167,112,79, 255});
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

void Game::handleGameOverEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }

        if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            //bool wasHoveringRetry = retryButton.isHovered;
            //bool wasHoveringMenu = menuButton.isHovered;
            retryButton.isHovered = isMouseOver(retryButton.rect, mouseX, mouseY);
            menuButton.isHovered = isMouseOver(menuButton.rect, mouseX, mouseY);

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (retryButton.isHovered) {
                    Mix_PlayChannel(-1, clickSound, 0);
                    state = GameState::PLAYING;
                    spawnEnemies();
                    generateWalls();
                    resetGame();
                }
                else if (menuButton.isHovered) {
                    resetGame();
                    Mix_PlayChannel(-1, clickSound, 0);
                    state = GameState::MAIN_MENU;
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
        renderText("MENU", 350, 400,{167,112,79, 255});
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    }
    // Position menu button
    menuButton.rect = {280, 410, 250, 200};

    // Render button
    SDL_RenderCopy(renderer,
                  menuButton.isHovered ? menuButton.hover : menuButton.normal,
                  nullptr, &menuButton.rect);

    SDL_RenderPresent(renderer);
}

void Game::handleVictoryEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }

        if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            menuButton.isHovered = isMouseOver(menuButton.rect, mouseX, mouseY);

            if (menuButton.isHovered && e.type == SDL_MOUSEBUTTONDOWN) {
                Mix_PlayChannel(-1, clickSound, 0);
                resetGame();
                state = GameState::MAIN_MENU;
            }
        }
    }
}

void Game::resetGame() {
    // Reset player
    player = PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE, (MAP_HEIGHT-2)*TILE_SIZE,renderer);

    // Regenerate walls
    walls.clear();
    generateWalls();

    // Respawn enemies
    spawnEnemies();
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

void Game::updateTankAnimation() {
    // Implement your tank animation logic here
    // This is just a placeholder
    static float animTime = 0;
    animTime += 0.1f;
    // Update any animation variables here
}

void Game::renderTank() {
    // Implement your tank rendering logic here
    if (tankGuideTexture) {
        SDL_Rect rect = {100, 150, 200, 200}; // Example position
        SDL_RenderCopy(renderer, tankGuideTexture, nullptr, &rect);
    }
}
