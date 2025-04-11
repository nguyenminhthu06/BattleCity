#include<iostream>
#include"Game.h"

Game::Game():player(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE)
    {
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
        //player = PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE);
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
                }
            }
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
                SDL_RenderFillRect(renderer,&tile);
            }
        }
        for(size_t i=0;i<walls.size();i++)
        {
            walls[i].render(renderer);
        }

        player.render(renderer);

        SDL_RenderPresent(renderer);
    }
void Game::run()
    {
       while(running)
       {
           handleEvents();
           render();
           SDL_Delay(16);
       }
    }
Game::~Game()
    {
       SDL_DestroyRenderer(renderer);
       SDL_DestroyWindow(window);
       SDL_Quit();
    }

