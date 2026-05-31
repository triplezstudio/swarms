#include <app.hh>
#include <iostream>
#include <text_render.hh>
#include <vulkan_renderer.hh>
#include <window.hh>
#include <ui.hh>


namespace tz
{
namespace rv = render::vulkan;

App::App(int width, int height, const std::string& title) :inputSystem(tz::input::SDL2InputSystem::getInstance())
{

  window = new tz::Window(width, height, title);
  renderer = new rv::Renderer(window);
  masterPipelineLayout = new MasterPipelineLayout(*renderer);
  textureAssetManager = new tz::TextureAssetManager(*renderer, masterPipelineLayout->getDiffuseTextureDescriptorSet());
  textRenderer = new tz::render::TextRenderer(*renderer, *textureAssetManager);

  immediateCommandProcessor = new ImmediateCommandProcessor(*renderer, *textRenderer, *masterPipelineLayout);

}

tz::render::vulkan::Renderer * App::vulkanRenderer()
{
  return dynamic_cast<tz::render::vulkan::Renderer *>(renderer);
}


void tz::App::run()
{
  while (true)
  {
    window->pollEvents();
    inputSystem.update(window->frameInputEvents);
    updateInputListeners();
    updateFrameListeners(16.66f);

    renderFrame();
  }

}

void App::updateInputListeners()
{
  for (auto& inputListener : inputListeners)
  {
    inputListener(inputSystem);
  }
}

tz::UISystem& App::createUISystem(int x, int y, int width, int height)
{
  auto uiSystem = new tz::UISystem(std::move(tz::UIHost{window,
                                                       renderer, textRenderer,
                                                       &inputSystem,
                                                       {x, y}, {width, height}}));

  uiSystems.push_back(uiSystem);
  return *uiSystem;
}


std::vector<tz::render::vulkan::CommandBuffer*> App::recordUICommandBuffers()
{
  std::vector<tz::render::vulkan::CommandBuffer*> frameCommandBuffers;
  for (auto& uiSystem: uiSystems)
  {
    frameCommandBuffers.push_back(&uiSystem->recordFrameCommandBuffer());
  }

  return frameCommandBuffers;

}


std::vector<tz::render::vulkan::CommandBuffer*> App::recordCommandBuffesForScenes()
{
  std::vector<tz::render::vulkan::CommandBuffer*> frameCommandBuffers;
  for (auto& scene: layerSortedScenes)
  {
    frameCommandBuffers.push_back(&scene->recordFrameCommandBuffer());
  }

  return frameCommandBuffers;
}

render::vulkan::CommandBuffer &App::recordImmediateCommandBuffers()
{
  return immediateCommandProcessor->recordFrameCommandBuffer();
}

void App::renderFrame()
{
  renderer->beginFrame();

  static rv::CommandBuffer* mainFrameCommandBuffer = renderer->createCommandBuffer();
  renderer->beginCommandBuffer(mainFrameCommandBuffer, true);
  renderer->endCommandBuffer(mainFrameCommandBuffer);

  std::vector<rv::CommandBuffer*> frameCommandBuffers;
  frameCommandBuffers.push_back(mainFrameCommandBuffer);

  auto sceneCommandBuffers = recordCommandBuffesForScenes();
  auto& immediateCommandBuffer = recordImmediateCommandBuffers();
  auto uiCommandBuffers = recordUICommandBuffers();

  sceneCommandBuffers.push_back(&immediateCommandBuffer);
  // temp debug:
  //sceneCommandBuffers.clear();
  for (auto & cb: sceneCommandBuffers)
  {
    frameCommandBuffers.push_back(cb);
  }


  for (auto& cb : uiCommandBuffers)
  {
    frameCommandBuffers.push_back(cb);
  }

  renderer->submitCommandBuffers(frameCommandBuffers);

  renderer->endFrame();

}

void App::addInputListener(tz::InputListener inputListener)
{
  inputListeners.push_back(inputListener);
}

void App::addUpdateListener(tz::FrameListener frameListener)
{
  frameListeners.push_back(frameListener);
}

void App::updateFrameListeners(float frameTime)
{

  for (auto& frameListenerFunc : frameListeners) {
    frameListenerFunc(this);
  }

}


void App::addScene(const std::string &name, Scene* scene, uint32_t layer)
{
  scenes.insert({name, scene});
  layerSortedScenes.push_back(scene);
  std::sort(layerSortedScenes.begin(), layerSortedScenes.end(), [this](Scene* a, Scene* b)
             {
               return sceneLayerMap[a] < sceneLayerMap[b];
             });
}

}







