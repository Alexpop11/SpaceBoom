#pragma once
#include "Entity.h"

class Bomb : public Entity {
public:
   Bomb(const std::string& name, float x, float y);
   virtual void tickUpdate() override;
   virtual void kick(bool hitWall) override;
   int          ExplodeTick;
   void         explode();
};
