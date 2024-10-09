
#include <webgpu/webgpu.hpp>

#include "rendering/RenderPipeline.h"
#include "rendering/Buffer.h"
#include "rendering/BindGroupLayout.h"
#include "rendering/DataFormats.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Application {
public:
   // Initialize everything and return true if it went all right
   Application();

   // Uninitialize everything that was initialized
   void Terminate();

   // Return true as long as the main loop should keep on running
   bool IsRunning();

   static Application& get();

   bool        initialized;
   std::string res_path;

   wgpu::TextureFormat          getSurfaceFormat() { return surfaceFormat; }
   wgpu::Device                 getDevice() { return device; }
   wgpu::Surface                getSurface() { return surface; }
   wgpu::Queue                  getQueue() { return queue; }
   Buffer<float>&               getPointBuffer() { return pointBuffer; }
   Buffer<uint16_t>&            getIndexBuffer() { return indexBuffer; }
   UniformBuffer<StarUniforms>& getUniformBuffer() { return uniformBuffer; }

   wgpu::TextureView GetNextSurfaceTextureView();

private:
   void InitializeResPath();


   GLFWwindow*                          window;
   wgpu::Instance                       instance;
   wgpu::Surface                        surface;
   wgpu::Adapter                        adapter;
   wgpu::Device                         device;   std::unique_ptr<wgpu::ErrorCallback> uncapturedErrorCallbackHandle;
   wgpu::Queue                          queue;
   wgpu::TextureFormat                  surfaceFormat;
   Buffer<float>                        pointBuffer;
   Buffer<uint16_t>                     indexBuffer;
   UniformBuffer<StarUniforms>          uniformBuffer;
};
