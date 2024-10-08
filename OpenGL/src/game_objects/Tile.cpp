#include "Tile.h"

Tile::Tile(const std::string& name, bool wall, bool unbreakable, float x, float y)
   : SquareObject(name, DrawPriority::Floor, x, y, "textures/alt-wall-bright.png")
   , wall(wall)
   , unbreakable(unbreakable) {
   wallTextureUnbreakable = Texture::create(Renderer::ResPath() + "textures/alt-wall-unbreakable.png");
   wallTexture            = Texture::create(Renderer::ResPath() + "textures/alt-wall-bright.png");

   // floor textures array
   std::vector<std::string> floorTextures = {"textures/2-alt-floor.png", "textures/2-alt-floor-2.png"};

   // alternate floor textures i made. I'm going to leave them here for convenience, just comment out the above line and
   // uncomment this one std::vector<std::string> floorTextures = {"Textures/alt-floor.png", "Textures/alt-floor-2.png",
   //                                          "Textures/alt-floor-2.png", "Textures/alt-floor-3.png"};

   // randomly select the texture
   floorTexture = Texture::create(Renderer::ResPath() + floorTextures[rand() % floorTextures.size()]);


   setTexture();
}

Tile::Tile(const std::string& name, float x, float y)
   : Tile(name, false, true, x, y) {}

void Tile::explode() {
   if (!unbreakable || !wall) {
      tintColor = {0.8, 0.5, 0.5, 0.9};
      wall      = false;
   }
}

std::vector<glm::vec2> Tile::getBounds() {
   if (wall) {
      std::vector<glm::vec2> bounds = {
         position + glm::vec2{-0.5, -0.5},
         position + glm::vec2{0.5,  -0.5},
         position + glm::vec2{0.5,  0.5 },
         position + glm::vec2{-0.5, 0.5 },
      };
      return bounds;
   } else {
      return std::vector<glm::vec2>{};
   }
}


void Tile::update() {
   tintColor.a = zeno(tintColor.a, 0, 0.1);
   setTexture();
}

void Tile::setTexture() {
   if (wall) {
      if (unbreakable) {
         texture = wallTextureUnbreakable;
      } else {
         texture = wallTexture;
      }
   } else {
      texture = floorTexture;
   }
}
