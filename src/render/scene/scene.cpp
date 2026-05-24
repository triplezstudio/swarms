
#include "include/scene.hh"

/*
 * We prepare all needed vulkan and other render helper objects
 * we need later when rendering a frame.
 */
tz::Scene::Scene(Camera &camera, rv::Renderer& renderer) : camera(camera), renderer(renderer)
{

  createMasterPipelineLayout();
  //prepareRenderPrimitives();
  psoCache = new tz::render::vulkan::PipelineStateObjectCache();
  commandBuffer = renderer.createCommandBuffer();


}



void tz::Scene::createMasterPipelineLayout()
{


}


void tz::Scene::addNode(SceneNode &node)
{
  nodes.push_back(node);
}


/**
 * This method creates render commands based on the current nodes, lights and camera.
 * @return A list of commands which should be self-contained to be sent as is to the GPU
 *         for rendering.
 */
std::vector<tz::render::vulkan::Command> tz::Scene::getRenderCommands()
{
  for (auto& sn : nodes)
  {
    auto pos = sn.transform.position;
  }
  return std::vector<tz::render::vulkan::Command>();
}
