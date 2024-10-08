#include "Background.h"
#include <array>

Background::Background(const std::string& name)
   : GameObject(name, DrawPriority::Background, {0, 0}) {
   shader = Shader::create(Renderer::ResPath() + "shaders/stars.shader");

   std::array<float, 8> positions = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};

   std::array<uint32_t, 6> indices = {0, 1, 2, 2, 3, 0};

   vb = VertexBuffer::create(positions);
   VertexBufferLayout layout;
   layout.Push<float>(2);
   va = std::make_shared<VertexArray>(vb, layout);
   ib = std::make_shared<IndexBuffer>(IndexBuffer(indices));
}

void Background::setUpShader(Renderer& renderer) {
   GameObject::setUpShader(renderer);
   auto [width, height] = renderer.WindowSize();
   shader->SetUniform2f("u_Resolution", {(float)width, (float)height});
}

void Background::render(Renderer& renderer) {
   GameObject::render(renderer);

   // draw if va, ib, and shader are set:
   if (va && ib && shader) {
      renderer.Draw(*va, *ib, *shader);
   }
}

void Background::update() {
   // Update logic for Background
}
