#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <optional>
#include <memory>
#include "glm/glm.hpp"

#include "../Renderer.h"
#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../IndexBuffer.h"
#include "../VertexArray.h"
#include "../Shader.h"

enum class DrawPriority {
   Background,
   Floor,
   Bomb,
   Character,
   Fog,
   UI,
};

class GameObject {
public:
   GameObject(const std::string& name, DrawPriority drawPriority, glm::vec2 position);
   GameObject(GameObject&& mE)            = default;
   GameObject& operator=(GameObject&& mE) = default;


   virtual ~GameObject() = default;
   bool  ShouldDestroy   = false;

   virtual std::vector<GameObject*> children() { return {}; }

   virtual void setUpShader(Renderer& renderer);
   virtual void render(Renderer& renderer);
   virtual void update();
   virtual void tickUpdate();


   std::shared_ptr<Shader>       shader;
   std::shared_ptr<VertexArray>  va;
   std::shared_ptr<VertexBuffer> vb;
   std::shared_ptr<IndexBuffer>  ib;

   std::string  name;
   DrawPriority drawPriority;
   glm::vec2    position;
   float        rotation = 0;
   float        scale    = 1.0f;

private:
   // Add any private members here if needed
};
