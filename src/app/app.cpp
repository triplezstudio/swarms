#include <app.hh>
#include <iostream>
#include <text_render.hh>
#include <vulkan_renderer.hh>
#include <window.hh>


namespace tz
{
namespace rv = render::vulkan;

App::App(int width, int height, const std::string& title) :inputSystem(tz::input::SDL2InputSystem::getInstance())
{

  window = new tz::Window(width, height, title);
  renderer = new rv::Renderer(window);
  textureAssetManager = new tz::TextureAssetManager(*renderer);
  textRenderer = new tz::render::TextRenderer(*renderer, *textureAssetManager);
  immediateCommandProcessor = new ImmediateCommandProcessor(*renderer);



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
    updateFrameListeners(16.66f);
    updateInputListeners();
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

void App::renderScenes()
{
  for (auto& scene: layerSortedScenes)
  {
    auto renderCommands = scene->getRenderCommands();
    for (auto& rc : renderCommands)
    {

    }
  }
}

void App::renderImmediateCommands()
{

  immediateCommandProcessor->render();


}


void App::renderFrame()
{
  renderer->beginFrame();

  renderScenes();
  renderImmediateCommands();

  renderer->endFrame();

}

void App::setUpdateFunction(tz::FrameListener frameListener)
{
  frameListeners.push_back(frameListener);
}

void App::updateFrameListeners(float frameTime)
{

  for (auto& frameListenerFunc : frameListeners) {
    frameListenerFunc(this);
  }



}
float App::getLastFrameTime()
{
  return 16.667f;
}
uint32_t App::createTexture(const std::string &imagePath)
{
  auto bitmapData = rv::loadBitmapDataFromPath(imagePath);
  auto image = renderer->createImage(bitmapData);
  auto texture = renderer->createTexture(image);
  renderer->updateTextureDescriptorSet(diffuseTextureDescriptorSet, 0, globalTextureIndex, texture);
  return globalTextureIndex++;
}

void tz::App::setInputListenerFunc(InputListener il) {
  this->inputListener = il;
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







