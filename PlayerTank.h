
#pragma once
#include "Tank.h"
#include "Wall.h"
#include <vector>

class PlayerTank : public Tank {
public:
    PlayerTank(int startX, int startY);

    void move(int dx, int dy, const std::vector<Wall>& walls);
    void render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Texture* bulletTexture) const;
};
