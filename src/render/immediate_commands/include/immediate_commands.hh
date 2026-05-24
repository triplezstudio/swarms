//
// Created by mgrus on 24.05.2026.
//

#ifndef SWARMS_IMMEDIATE_COMMANDS_HH
#define SWARMS_IMMEDIATE_COMMANDS_HH
#include <Eigen/Dense>
#include <render_helpers.hh>

namespace tz
{

class TZ_API ImmediateCommandProcessor
{

  public:
  ImmediateCommandProcessor(tz::render::vulkan::Renderer& renderer);
  void activate3DCamera();
  void activate3DCamera(Eigen::Vector3f position, Eigen::Vector3f lookAt);
  void activateUICamera();
  void activateUICamera(Eigen::Vector3f position);

  void renderQuad(Transform transform, RenderHints renderHints = {});
  void renderCube(Transform transform, RenderHints renderHints = {});
  void renderSphere(Transform transform, RenderHints renderHints = {});
  void renderCylinder(Transform transform, RenderHints renderHints = {});

  void render();

  private:
      Camera* default3DCamera = nullptr;
      Camera* defaultUICamera = nullptr;
      Camera* activeRenderCamera = nullptr;

      std::vector<PrimitiveRenderData> framePrimitives;
      rv::Buffer* quadPosVertexBuffer       = nullptr;
      rv::Buffer* cubePosVertexBuffer = nullptr;
      rv::Buffer* cubePosTexCoordVertexBuffer = nullptr;
      rv::Buffer* quadPosTexCoordVertexBuffer = nullptr;
      rv::Buffer* quadIndexBuffer = nullptr;
      rv::Buffer* cubeIndexBuffer = nullptr;
      rv::Buffer* cubeTexIndexBuffer = nullptr;

      rv::PipelineStateObjectCache psoCache;
      rv::PipelineStateObject* colorOnlyPSO = nullptr;
      rv::CommandBuffer* commandBuffer = nullptr;

      rv::DescriptorSet* cameraDescriptorSet = nullptr;
      rv::DescriptorSet* perObjectDescriptorSet = nullptr;
      rv::DescriptorSet* diffuseTextureDescriptorSet = nullptr;
      rv::PipelineLayout* masterPipelineLayout = nullptr;

      render::vulkan::Renderer &renderer;

      void buildPSOCache();
      void createMasterPipelineLayout();
      std::vector<PrimitiveRenderData> getRenderPrimitivesByCamera(Camera *camera);
      void renderPrimitives(const std::vector<PrimitiveRenderData> &primitives,
                            uint32_t &primitiveCounter);

      render::vulkan::PipelineStateObject *createTextPSO();
      render::vulkan::PipelineStateObject *createTexturedPSO();
      render::vulkan::PipelineStateObject *createColorOnlyPSO();
};
}




#endif //SWARMS_IMMEDIATE_COMMANDS_HH
