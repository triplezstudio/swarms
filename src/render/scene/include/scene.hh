//
// Created by Martin Gruscher on 19.05.26.
//

#ifndef SWARMS_SCENE_HH
#define SWARMS_SCENE_HH
#include "immediate_commands.hh"
#include <render_helpers.hh>
#include <text_render.hh>

namespace tz
{

/**
 *
 */
class SceneNode
{
  public:
  Transform transform;
  PrimitiveGeometryType geometryType;
};

/**
 * A scene consists of a camera,
 * lights and renderable objects.
 * The application may consist of several scenes,
 * e.g. one with a 2d camera for the ui,
 * one with a 3d camera for the main scene,
 * and maybe another one in 3d, which "films" the world from above
 * to produce a top-down mini-map.
 *
 * Each scene has a recordAndSubmitFrameCommandBuffer target, either the main framebuffer,
 * so it would just show up directly on the display, or a
 * recordAndSubmitFrameCommandBuffer-texture.
 * For the example above, the top-down-scene would recordAndSubmitFrameCommandBuffer into a texture,
 * which the 2d ui scene would then use to draw the mini-map in a certain are of the screen.
 */
class TZ_API Scene
{
public:
  Scene(Camera& camera, rv::Renderer& renderer);
  void addNode(SceneNode& node);
  void removeNode(SceneNode& node);

  void render();

  // Records the commands into the commandbuffer for the current frame,
  // but does not submit the cmdbuffer to the queue.
  // This allows for deferred submission and collecting different comandbuffers
  // and submit them in a batch.
  render::vulkan::CommandBuffer &recordFrameCommandBuffer();


private:
  Camera& camera;
  rv::Renderer &renderer;

  std::vector<SceneNode> nodes;

  // Render utility objects:
  std::vector<uint32_t> quadIndices;
  std::vector<uint32_t> cubeIndices;
  std::vector<uint32_t> cubeIndicesPosTex;
  rv::Buffer* quadPosVertexBuffer       = nullptr;
  rv::Buffer* cubePosVertexBuffer = nullptr;
  rv::Buffer* cubePosTexCoordVertexBuffer = nullptr;
  rv::Buffer* quadPosTexCoordVertexBuffer = nullptr;
  rv::Buffer* quadIndexBuffer = nullptr;
  rv::Buffer* cubeIndexBuffer = nullptr;
  rv::Buffer* cubeTexIndexBuffer = nullptr;
  rv::Buffer* tzLabelVertexBuffer = nullptr;
  rv::Buffer* tzLabelIndexBuffer = nullptr;
  std::map<std::string, tz::render::TextGeometry> textGeometries;
  std::map<std::string, rv::Buffer*> textVertexBuffers;
  std::map<std::string, rv::Buffer*> textIndexBuffers;
  std::map<int, uint32_t> fontTextureMap;
  uint32_t tzLabelIndexCount = 0;
  rv::PipelineStateObject* colorOnlyPSO = nullptr;
  rv::CommandBuffer* commandBuffer = nullptr;
  std::vector<PrimitiveRenderData> framePrimitives;

  rv::DescriptorSet* cameraDescriptorSet = nullptr;
  rv::DescriptorSet* perObjectDescriptorSet = nullptr;
  rv::DescriptorSet* diffuseTextureDescriptorSet = nullptr;
  MasterPipelineLayout* masterPipelineLayout = nullptr;
  rv::PipelineStateObjectCache *psoCache = nullptr;

  Camera* default3DCamera = nullptr;
  Camera* defaultUICamera = nullptr;
  Camera* activeRenderCamera = nullptr;

  ImmediateCommandProcessor *immediateCommandProcessor = nullptr;

};


} // namespace tz

#endif //SWARMS_SCENE_HH
