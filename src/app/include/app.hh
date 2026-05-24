#include <scene.hh>
#include <Eigen/Dense>
#include <defines.h>
#include <functional>
#include <input.hh>
#include <render_helpers.hh>
#include <string>
#include <text_render.hh>
#include <vulkan_renderer.hh>
#include <immediate_commands.hh>
#include <window.hh>

namespace tz {
namespace rv =  render::vulkan;

struct alignas(16) TransformUniformBufferObject
{
  Eigen::Matrix4f model;

};










  class App;
  using FrameListener = std::function<void(App* app)>;
  using InputListener = std::function<void(const tz::input::SDL2InputSystem& inputSystem)>;

  class TZ_API App
  {


    public:
    App(int width, int height, const std::string& title);

    render::vulkan::Renderer& getRenderer() { return *renderer; }
    render::TextRenderer& getTextRenderer() { return *textRenderer; };
    TextureAssetManager& getTextureAssetManager() { return *textureAssetManager; };
    ImmediateCommandProcessor& getImmediateCommandProcessor() { return *immediateCommandProcessor; }

    virtual void run();
    virtual void setUpdateFunction(FrameListener frameListener);
    virtual void setInputListenerFunc(InputListener il);
    virtual float getLastFrameTime();

    ImmediateCommandProcessor* immediateCommandProcessor = nullptr;

    uint32_t createTexture(const std::string& imagePath);

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
    void addScene(const std::string& name, Scene* scene, uint32_t layer);

  private:
      tz::input::SDL2InputSystem& inputSystem;
      rv::Renderer* renderer = nullptr;
      tz::TextureAssetManager* textureAssetManager = nullptr;
      tz::render::TextRenderer* textRenderer = nullptr;
      tz::MasterPipelineLayout* masterPipelineLayout = nullptr;

      std::vector<FrameListener> frameListeners;
      std::vector<InputListener> inputListeners;

      rv::Buffer* tzLabelVertexBuffer = nullptr;
      rv::Buffer* tzLabelIndexBuffer = nullptr;
      std::map<std::string, tz::render::TextGeometry> textGeometries;
      std::map<std::string, rv::Buffer*> textVertexBuffers;
      std::map<std::string, rv::Buffer*> textIndexBuffers;
      std::map<int, uint32_t> fontTextureMap;
      uint32_t tzLabelIndexCount = 0;

      void updateFrameListeners(float frameTime);

      void renderFrame();
      render::vulkan::Renderer *vulkanRenderer();

      void updateInputListeners();
      Window *window = nullptr;

      std::map<std::string, Scene*> scenes;
      std::vector<Scene*> layerSortedScenes;

      InputListener inputListener;

      std::map<Scene*, uint32_t> sceneLayerMap;
      void renderScenes();
      void renderImmediateCommands();
      std::vector<render::vulkan::CommandBuffer *> recordCommandBuffesForScenes();
      render::vulkan::CommandBuffer &recordImmediateCommandBuffers();
  };


}

