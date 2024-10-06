#include <iostream>
#include <fstream>
#include "World.h"
#include <algorithm>

#include "Renderer.h"
#include "game_objects/Player.h"
#include "game_objects/Background.h"
#include "game_objects/Camera.h"
#include "game_objects/Tile.h"
#include "game_objects/enemies/Bomber.h"
#include "game_objects/enemies/Turret.h"
#include "game_objects/Mine.h"

std::vector<std::unique_ptr<GameObject>> World::gameobjects      = {};
std::vector<std::unique_ptr<GameObject>> World::gameobjectstoadd = {};
float World::timeSpeed = 1.0f;
bool World::settingTimeSpeed = false;
bool World::shouldTick = false;

void World::LoadMap(const std::string& map_path) {
   gameobjects.clear();

   std::ifstream file(Renderer::ResPath() + map_path);

   if (!file.is_open()) {
      std::cerr << "Error opening file: " << map_path << std::endl;
   }

   std::vector<std::string> lines;
   std::string              line;

   // Read all lines into a vector
   while (std::getline(file, line)) {
      lines.push_back(std::move(line));
   }

   size_t total_rows = lines.size();

   for (size_t row = 0; row < total_rows; ++row) {
      for (unsigned long x = 0; x < lines[row].length(); ++x) {
         char   c = lines[row][x];
         size_t y = total_rows - row;
         if (c != '\n') {
            if (c == 'b') { // Background
               gameobjects.push_back(std::make_unique<Background>(Background("Background")));
            }
            if (c == 'p') { // player
               gameobjects.push_back(std::make_unique<Player>(Player("Coolbox", (float)x, (float)y)));
               gameobjects.push_back(std::make_unique<Tile>(Tile("Floor", (float)x, (float)y)));
            }
            if (c == 'f') { // floor
               gameobjects.push_back(std::make_unique<Tile>(Tile("Floor", (float)x, (float)y)));
            }
            if (c == 'w') { // wall
               gameobjects.push_back(std::make_unique<Tile>(Tile("Wall", true, false, (float)x, (float)y)));
            }
            if (c == 'W') { // wall
               gameobjects.push_back(std::make_unique<Tile>(Tile("Wall", true, true, (float)x, (float)y)));
            }
            if (c == 'e') { // enemy Bomber
               gameobjects.push_back(std::make_unique<Bomber>(Bomber("bomber", (float)x, (float)y)));
               gameobjects.push_back(std::make_unique<Tile>(Tile("Floor", (float)x, (float)y)));
            }
            if (c == 't') { // Turret
               gameobjects.push_back(std::make_unique<Turret>(Turret("turret", (float)x, (float)y)));
               gameobjects.push_back(std::make_unique<Tile>(Tile("Floor", (float)x, (float)y)));
            }
            if (c == 'm') { // Mine
               gameobjects.push_back(std::make_unique<Mine>(Mine("mine", (float)x, (float)y)));
               gameobjects.push_back(std::make_unique<Tile>(Tile("Floor", (float)x, (float)y)));
            }
         }
      }
   }
}

void sortGameObjectsByPriority(std::vector<std::unique_ptr<GameObject>>& gameObjects) {
   std::sort(gameObjects.begin(), gameObjects.end(),
             [](const std::unique_ptr<GameObject>& a, const std::unique_ptr<GameObject>& b) {
                return a->drawPriority < b->drawPriority;
             });
}

void sortGameObjectsByPriority(std::vector<GameObject*>& gameObjects) {
   std::sort(gameObjects.begin(), gameObjects.end(),
             [](const GameObject* a, const GameObject* b) { return a->drawPriority < b->drawPriority; });
}


void World::UpdateObjects() {
   auto objects = get_gameobjects();
   sortGameObjectsByPriority(objects);

   for (auto& gameobject : objects) {
      gameobject->update();
   }

   // erase dead objects
   // ------------------
   std::erase_if(World::gameobjects, [](const auto& gameobject) { return gameobject->ShouldDestroy; });

   // add newly created objects
   // -------------------------
   for (auto& o : World::gameobjectstoadd)
      World::gameobjects.push_back(std::move(o));
   World::gameobjectstoadd.clear();
}

void World::TickObjects() {
   auto objects = get_gameobjects();
   sortGameObjectsByPriority(objects);

   for (auto& gameobject : objects) {
      gameobject->tickUpdate();
   }
}

void World::RenderObjects(Renderer& renderer) {
   auto objects = get_gameobjects();
   sortGameObjectsByPriority(objects);

   for (auto& gameobject : objects) {
      gameobject->render(renderer);
   }
}
