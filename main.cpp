#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include<Wall.h>
#include "constants.h"
#include<Bullet.h>
#include<Tank.h>
#include<PlayerTank.h>
#include<EnemyTank.h>
#include<Game.h>
using namespace std;



int main() {
    srand(time(0));
    Game game;
    if (game.running) {
        game.run();
    }
    return 0;
}
