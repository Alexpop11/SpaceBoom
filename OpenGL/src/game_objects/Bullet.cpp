#include "Bullet.h"
#include "../World.h"
#include "Tile.h"
#include "Player.h"

Bullet::Bullet(const std::string& name, float x, float y, int direction_x, int direction_y)
   : SquareObject(name, 4, x, y)
   , direction_x(direction_x)
   , direction_y(direction_y) {
   r = 0.3f;
   g = 0.2f;
   b = 0.2f;
}

void Bullet::tickUpdate() {
   // Move the bullet
   tile_x += direction_x;
   tile_y += direction_y;

   // Check if the bullet hits a wall
   auto nearbyWalls = World::at<Tile>(tile_x, tile_y);
   if (!nearbyWalls.empty()) {
      ShouldDestroy = true;
      return; // Stop further processing since the bullet is destroyed
   }

   // Assuming you have a way to check for characters at the new position
   auto nearbyCharacters = World::at<Player>(tile_x, tile_y);
   if (!nearbyCharacters.empty()) {
      auto* character = nearbyCharacters.front(); // Assuming we target the first found character
      character->health -= 1;
      std::cout << "Bullet damaged player. Their health is now " << character->health << std::endl;
      ShouldDestroy = true;
   }
}
