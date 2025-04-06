#include "Tank.h"
#include "constants.h"
#include <algorithm>

Tank::Tank(int startX, int startY)
{
    x = startX;
    y = startY;
    rect.x = x;
    rect.y = y;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
    dirX = 0;
    dirY = -1;
}

void Tank::shoot()
{
    bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY));
}

void Tank::updateBullets()
{
    for (auto& bullet : bullets)
    {
        bullet.move();
    }
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                      [](const Bullet& b) { return !b.active; }),  // Sửa Bullets -> Bullet&
        bullets.end()
    );
}
