#include "ui.hh"
#include <Eigen/Dense>
#include <defines.h>
#include <functional>
#include <immediate_commands.hh>
#include <input.hh>
#include <render_helpers.hh>
#include <scene.hh>
#include <string>
#include <text_render.hh>
#include <vulkan_renderer.hh>
#include <window.hh>

namespace tz {
namespace rv = render::vulkan;

class App;
using FrameListener = std::function<void(App *app)>;
using InputListener = std::function<void(tz::input::SDLInputSystem &inputSystem)>;

class TZ_API App
{
  public:
  App(int width, int height, const std::string &title);

  Window &getWindow()
  {
    return *window;
  }
  render::vulkan::Renderer &getRenderer()
  {
    return *renderer;
  }
  render::TextRenderer &getTextRenderer()
  {
    return *textRenderer;
  };
  TextureAssetManager &getTextureAssetManager()
  {
    return *textureAssetManager;
  };
  ImmediateCommandProcessor &getImmediateCommandProcessor()
  {
    return *immediateCommandProcessor;
  }
  input::SDLInputSystem &getInputSystem()
  {
    return inputSystem;
  }
  tz::UISystem &createUISystem(int x, int y, int width, int height);

  virtual void run();
  virtual void addUpdateListener(FrameListener frameListener);
  virtual void addInputListener(InputListener inputListener);

  ImmediateCommandProcessor *immediateCommandProcessor = nullptr;

  /**
     * Adds a scene under a given name and assigns it to a layer.
     * Layers are numbered front to back, so layer 1 is in front of layer 2.
     * To achieve the desired effect, scenes are rendered back to front,
     * so higher layers are rendered before lower numbers.
     * Scenes with identical layer numbers are rendered in an undefined sequence
     * relative to each other.
     *
     * @param name
     * @param scene
     * @param layer
     */
  void addScene(const std::string &name, Scene *scene, uint32_t layer);

  private:
  tz::input::SDLInputSystem &inputSystem;
  rv::Renderer *renderer                         = nullptr;
  tz::TextureAssetManager *textureAssetManager   = nullptr;
  tz::render::TextRenderer *textRenderer         = nullptr;
  tz::MasterPipelineLayout *masterPipelineLayout = nullptr;

  std::vector<FrameListener> frameListeners;
  std::vector<InputListener> inputListeners;

  std::map<int, uint32_t> fontTextureMap;
  uint32_t tzLabelIndexCount = 0;

  void updateFrameListeners(float frameTime);

  void renderFrame();
  render::vulkan::Renderer *vulkanRenderer();

  void updateInputListeners();
  Window *window = nullptr;

  std::map<std::string, Scene *> scenes;
  std::vector<Scene *> layerSortedScenes;

  InputListener inputListener;

  std::map<Scene *, uint32_t> sceneLayerMap;
  std::vector<render::vulkan::CommandBuffer *> recordCommandBuffesForScenes();
  render::vulkan::CommandBuffer &recordImmediateCommandBuffers();
  std::vector<render::vulkan::CommandBuffer *> recordUICommandBuffers();

  std::vector<UISystem *> uiSystems;
};

} // namespace tz
