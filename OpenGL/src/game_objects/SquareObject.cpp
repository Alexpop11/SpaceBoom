#include "SquareObject.h"

SquareObject::SquareObject(const std::string& name, int drawPriority, int x, int y)
   : GameObject(name, drawPriority, x, y) {
   r      = 0.5;
   g      = 0.5;
   b      = 0.5;
   tile_x = x;
   tile_y = y;
   shader = Shader::create(Renderer::ResPath() + "shaders/shader.shader");

   float positions[] = {
      -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
   };

   unsigned int indices[] = {0, 1, 2, 2, 3, 0};

   unsigned int vao;
   GLCall(glGenVertexArrays(1, &vao));
   GLCall(glBindVertexArray(vao));

   va = std::make_optional(VertexArray());
   vb = std::make_optional(VertexBuffer(positions, 4 * 2 * sizeof(float)));
   VertexBufferLayout layout;
   layout.Push<float>(2);
   va->AddBuffer(*vb, layout);

   ib = std::make_optional(IndexBuffer(indices, 6));
}

void SquareObject::setUpShader(Renderer& renderer) {
   GameObject::setUpShader(renderer);
   shader->SetUniform4f("u_Color", r, g, b, 1.0f);
}

void SquareObject::render(Renderer& renderer) {

   GameObject::render(renderer);

   // draw if va, ib, and shader are set:
   if (va && ib && shader) {
      renderer.Draw(*va, *ib, *shader);
   }
}

void SquareObject::update() {
   // Update logic for SquareObject
}
