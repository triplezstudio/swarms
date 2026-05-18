

#include <sdl2.hh>
#include <vulkan_renderer.hh>
#include <window_helpers.hh>
#include <window_system.hh>

static tz::render::vulkan::Renderer * renderer = nullptr;
static tz::render::vulkan::CommandBuffer * commandBuffer = nullptr;
static tz::WindowSystem * windowSystem = nullptr;

extern void TZ_API renderFrame();


void tz::beginDrawing()
{


}


void tz::endDrawing()
{
  windowSystem->pollEvents();
  //inputSystem.update(reinterpret_cast<SDL2WindowSystem*>(windowSystem)->getFrameEvents());
  //updateFrameListeners(16.66f);
  //updateInputListeners();
  renderFrame();

}

extern void initRenderer(tz::Window * wd);
void tz::initWindow(int width, int height, const std::string &title)
{

  windowSystem = new tz::SDL2WindowSystem();

  WindowDesc wd;
  wd.width = 1280;
  wd.height = 720;
  wd.title = title;
  auto window = windowSystem->createWindow(wd);
  initRenderer(window);

}
