#include"Game.h"
#include "Constants.h"
//using namespace std;
int main(int argc, char* argv[])
{
    Game game;
    if(game.running)
    {
        game.run();
    }
    return 0;
}
