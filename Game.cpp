#include<iostream>
#include<algorithm>
#include"Game.h"
#include<SDL_image.h>

Game::Game():player(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE)
    {
        inMenu=true;
        running = true;
        if(SDL_Init(SDL_INIT_VIDEO)<0)
        {
            std::cerr <<"SDL could not initialize! SDL_Error: "<<SDL_GetError() <<std::endl;
            running = false;
        }
        window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
        if(!window)
        {
            std::cerr <<"Window could not be created! SDL_Error: "<<SDL_GetError() <<std::endl;
            running = false;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(!renderer)
        {
            std::cerr <<"Renderer could not be created! SDL_Error: "<<SDL_GetError() <<std::endl;
            running = false;
        }
        generateWalls();
        spawnEnemies();
        initMenu();

        gameOverTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/loser.jpg");
        victoryTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/winner.jpg");

    // Khởi tạo nút Retry
        //retryButton.rect = {200, 200, 200, 200};
        retryButton.texture = loadTexture("C:/Users/Minh Tuan/Desktop/button.jpg");
        retryButton.hoverTexture = loadTexture("C:/Users/Minh Tuan/Desktop/button.jpg");

    // Khởi tạo nút Menu
       // menuButton.rect = {300, 400, 200, 200};
        menuButton.texture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button1.jpg");
        menuButton.hoverTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/button1.jpg");

        int buttonWidth = 200;
        int buttonHeight = 200;
        int marginBottom = 50;

// Tính khoảng cách ngang giữa 2 nút
        int totalSpacing = SCREEN_WIDTH - (2 * buttonWidth); // 800 - 400 = 400
        int spaceBetween = totalSpacing / 3;                // 400 / 3 ≈ 133

// Cài vị trí nút Retry (bên trái)
    retryButton.rect = {
        spaceBetween,                          // x
        SCREEN_HEIGHT - buttonHeight - marginBottom, // y
        buttonWidth,
        buttonHeight
    };

// Cài vị trí nút Menu (bên phải)
    menuButton.rect = {
        2 * spaceBetween + buttonWidth,        // x
        SCREEN_HEIGHT - buttonHeight - marginBottom, // y
        buttonWidth,
        buttonHeight
    };
        state = GameState::MENU;
        //player = PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE);
    }
SDL_Texture* Game::loadTexture(const std::string& path)
{
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture)
    {
        std::cerr << "Failed to load texture: " << path << " - " << IMG_GetError() << std::endl;
    }
    return texture;
}
void Game::generateWalls()
    {
        for(int i = 3; i< MAP_HEIGHT - 3; i+=2)
        {
            for(int j = 3; j< MAP_WIDTH - 3; j+=2)
            {
                Wall w = Wall(j * TILE_SIZE, i * TILE_SIZE);
                walls.push_back(w);
            }
        }
    }
void Game::handleEvents()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP: player.move(0, -5, walls);
                                  break;
                    case SDLK_DOWN: player.move(0, 5, walls);
                                  break;
                    case SDLK_LEFT: player.move(-5, 0, walls);
                                  break;
                    case SDLK_RIGHT: player.move(5, 0, walls);
                                  break;
                    case SDLK_SPACE: player.shoot(); break;

                }
            }
        }
    }
void Game::spawnEnemies()
{
    enemies.clear();
    for(int i = 0; i < enemyNumber; i++)
    {
        int ex,ey;
        bool validPosition = false;
        while(!validPosition)
        {
            ex = (rand() % (MAP_WIDTH -2)+1)*TILE_SIZE;
            ey = (rand() % (MAP_HEIGHT -2)+1)*TILE_SIZE;
            validPosition = true;
            for(const auto &wall : walls)
            {
                if(wall.active && wall.x == ex && wall.y == ey)
                {
                    validPosition = false;
                    break;
                }
            }
        }
        enemies.push_back(EnemyTank(ex,ey));
    }
}
void Game::render()
    {
        SDL_SetRenderDrawColor(renderer, 78, 176, 155, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 250, 224, 199, 255);
        for(int i = 1; i< MAP_HEIGHT - 1; i++)
        {
            for(int j = 1; j< MAP_WIDTH - 1; j++)
            {
                SDL_Rect tile = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                SDL_RenderFillRect(renderer, &tile);
            }
        }
        for(size_t i=0;i< walls.size();i++)
        {
            walls[i].render(renderer);
        }

        player.render(renderer);
        for(auto &enemy : enemies)
        {
            enemy.render(renderer);
        }

        SDL_RenderPresent(renderer);
    }

void Game::update()
{
    player.updateBullets();
    if (state != GameState::PLAYING) return;

    for(auto& bullet : player.bullets)
    {
        for(auto& enemy : enemies)
        {
            if(enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect))
            {
                enemy.active = false;
                bullet.active = false;
                break;
            }
        }
    }
    for(auto& enemy : enemies)
    {
        enemy.move(walls);
        enemy.updateBullets();
        if(rand() % 100 < 2)
        {
            enemy.shoot();
        }
    }
    for(auto& enemy : enemies)
    {
        for(auto& bullet : enemy.bullets)
        {
            for(auto& wall : walls)
            {
                if(wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect))
                {
                    wall.active = false;
                    bullet.active = false;
                    break;
                }
            }
        }
    }
    for(auto& bullet : player.bullets)
    {
        for(auto& wall : walls)
        {
            if(wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect))
            {
                wall.active = false;
                bullet.active = false;
                break;
            }
        }
    }
    for(auto& bullet : player.bullets)
    {
        for(auto& enemy : enemies)
        {
            if(enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect))
            {
                enemy.active = false;
                bullet.active = false;
            }
        }
    }
    enemies.erase(std::remove_if(enemies.begin(),enemies.end(),[](EnemyTank &e){return !e.active; }), enemies.end());

    if(enemies.empty())
    {
        state = GameState::VICTORY;
    }
    for(auto& enemy : enemies)
    {
        for(auto& bullet : enemy.bullets)
        {
            if(SDL_HasIntersection(&bullet.rect, &player.rect))
            {
                state = GameState::GAME_OVER;
                return;
            }
        }
    }
}
void Game::renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ tiêu đề game
   // SDL_Rect titleRect = {200, 100, 400, 100};
    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    //SDL_RenderFillRect(renderer, &titleRect);

    // Vẽ nút Play
    if(playButtonGame.isHovered) {
        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        SDL_Rect hoverRect = {playButtonGame.rect.x - 5, playButtonGame.rect.y - 5,
                             playButtonGame.rect.w + 10, playButtonGame.rect.h + 10};
        SDL_RenderFillRect(renderer, &hoverRect);
    }
    SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, NULL);

    // Vẽ nút Play
    SDL_Texture* currentBtnTexture = playButtonGame.isHovered ?
                                   playButtonGame.hoverTexture :
                                   playButtonGame.texture;

    SDL_RenderCopy(renderer, playButtonGame.texture, NULL, &playButtonGame.rect);
    SDL_RenderPresent(renderer);
}
void Game::run()
{
    while (running)
    {
        switch (state)
        {
            case GameState::MENU:
                handleMenuEvents();
                renderMenu();
                break;
            case GameState::INSTRUCTIONS:
                handleInstructionEvents();
                renderInstructions();
                break;
            case GameState::PLAYING:
                handleEvents();
                update();
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
        }
        SDL_Delay(16);
    }
}

void Game::initMenu()
{
    menuBackgroundTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/background1.png");

    if (!menuBackgroundTexture)
    {
        std::cerr << "Warning: Could not load menu background. Using fallback color.\n";
        SDL_Surface* surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                                   0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 30, 30, 50)); // Màu xanh đậm
        menuBackgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    instructionBgTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/Guide.jpg"); // Thêm dòng này

    instructionTexture = loadTexture("D:/A_Teaching/LTNC/2024/DEMO/Guide.jpg");
    if (!instructionTexture)
    {
        std::cerr << "Failed to load instruction image! Using fallback.\n";
    }

    playButtonGame.rect = {300, 400, 200, 200};
    playButtonGame.texture = loadTexture("C:/Users/Minh Tuan/Desktop/button.jpg");

    if (!playButtonGame.texture) {
        SDL_Surface* surface = SDL_CreateRGBSurface(0, 200, 80, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 128, 0));
        playButtonGame.texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void Game::renderInstructions() {
    // Clear màn hình
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ background nếu có
    if (instructionBgTexture) {
        SDL_RenderCopy(renderer, instructionBgTexture, NULL, NULL);
    }

    // Vẽ khung hướng dẫn (căn giữa màn hình)
    SDL_Rect instructionRect = {SCREEN_WIDTH,SCREEN_HEIGHT,800,600};


    // Vẽ nội dung hướng dẫn
    if (instructionTexture) {
        SDL_RenderCopy(renderer, instructionTexture, NULL, &instructionRect);
    }

    // Vẽ nút Play (điều chỉnh vị trí)
     playButtonMenu.rect = {300, 400, 200, 200};
    playButtonMenu.texture = loadTexture("C:/Users/Minh Tuan/Desktop/button.jpg");

    SDL_RenderCopy(renderer, playButtonMenu.texture, NULL, &playButtonMenu.rect);

    SDL_RenderPresent(renderer);
}

void Game::handleMenuEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }

            if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                playButtonGame.isHovered = (mouseX >= playButtonGame.rect.x &&
                                      mouseX <= playButtonGame.rect.x + playButtonGame.rect.w &&
                                      mouseY >= playButtonGame.rect.y &&
                                      mouseY <= playButtonGame.rect.y + playButtonGame.rect.h);

                if (playButtonGame.isHovered && e.type == SDL_MOUSEBUTTONDOWN)
                {
                    state = GameState::INSTRUCTIONS;
                }

            }
        }
    }

void Game::handleInstructionEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }

        if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            playButtonGame.isHovered = (mouseX >= playButtonGame.rect.x &&
                            mouseX <= playButtonGame.rect.x + playButtonGame.rect.w &&
                            mouseY >= playButtonGame.rect.y &&
                            mouseY <= playButtonGame.rect.y + playButtonGame.rect.h);

            if (playButtonGame.isHovered && e.type == SDL_MOUSEBUTTONDOWN)
            {
                state = GameState::PLAYING;
            }
        }
    }
}

Game::~Game()
    {
        if (menuBackgroundTexture) SDL_DestroyTexture(menuBackgroundTexture);
        if (playButtonMenu.texture) SDL_DestroyTexture(playButtonMenu.texture);
        if (gameOverTexture) SDL_DestroyTexture(gameOverTexture);
        if (victoryTexture) SDL_DestroyTexture(victoryTexture);
        if (retryButton.texture) SDL_DestroyTexture(retryButton.texture);
        if (retryButton.hoverTexture) SDL_DestroyTexture(retryButton.hoverTexture);
        if (menuButton.texture) SDL_DestroyTexture(menuButton.texture);
        if (menuButton.hoverTexture) SDL_DestroyTexture(menuButton.hoverTexture);
        SDL_DestroyTexture(playButtonGame.texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
void Game::renderGameOver() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (gameOverTexture) {
        SDL_RenderCopy(renderer, gameOverTexture, NULL, NULL);
    }

    int buttonSpacing = 85;
    int totalWidth = retryButton.rect.w + menuButton.rect.w + buttonSpacing;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int yPos = SCREEN_HEIGHT - 190;

    retryButton.rect.x = startX;
    retryButton.rect.y = yPos;

    menuButton.rect.x = startX + retryButton.rect.w + buttonSpacing;
    menuButton.rect.y = yPos;

    SDL_RenderCopy(renderer, retryButton.texture, NULL, &retryButton.rect);
    SDL_RenderCopy(renderer, menuButton.texture, NULL, &menuButton.rect);

    SDL_RenderPresent(renderer);
}


void Game::renderVictory() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (victoryTexture) {
        SDL_RenderCopy(renderer, victoryTexture, NULL, NULL);
    }

    // Căn giữa nút Menu
    menuButton.rect = {
        (SCREEN_WIDTH - menuButton.rect.w) / 2,  // center X
        SCREEN_HEIGHT - 190,                     // vị trí Y tùy chọn
        menuButton.rect.w,
        menuButton.rect.h
    };

    SDL_RenderCopy(renderer, menuButton.texture, NULL, &menuButton.rect);
    SDL_RenderPresent(renderer);
}


void Game::handleGameOverEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Kiểm tra nút Retry
            if (mouseX >= retryButton.rect.x && mouseX <= retryButton.rect.x + retryButton.rect.w &&
                mouseY >= retryButton.rect.y && mouseY <= retryButton.rect.y + retryButton.rect.h) {
                resetGame();
                state = GameState::PLAYING;
            }

            // Kiểm tra nút Menu
            if (mouseX >= menuButton.rect.x && mouseX <= menuButton.rect.x + menuButton.rect.w &&
                mouseY >= menuButton.rect.y && mouseY <= menuButton.rect.y + menuButton.rect.h) {
                resetGame();
                state = GameState::MENU;
            }
        }
    }
}

void Game::handleVictoryEvents() {
    handleGameOverEvents(); // Dùng chung logic
}

void Game::resetGame() {
    player = PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE, (MAP_HEIGHT-2)*TILE_SIZE);

    walls.clear();
    generateWalls();
    spawnEnemies();
}
