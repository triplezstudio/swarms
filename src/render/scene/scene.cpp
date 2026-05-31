
#include "include/scene.hh"
#include <immediate_commands.hh>

/*
 * We prepare all needed vulkan and other render helper objects
 * we need later when rendering a frame.
 */
tz::Scene::Scene(Camera &camera, rv::Renderer& renderer, tz::render::TextRenderer& textRenderer)
  : camera(camera), renderer(renderer), textRenderer(textRenderer)
{

  masterPipelineLayout = new MasterPipelineLayout(renderer);
  immediateCommandProcessor = new tz::ImmediateCommandProcessor(renderer, textRenderer,  *masterPipelineLayout);

}


void tz::Scene::addNode(SceneNode &node)
{
  nodes.push_back(node);
}

tz::render::vulkan::CommandBuffer& tz::Scene::recordFrameCommandBuffer()
{
  if (camera.type == CameraType::Perspective)
  {
    immediateCommandProcessor->activate3DCamera(camera.pos, camera.lookAt);
  }
  else
  {
    immediateCommandProcessor->activateUICamera(camera.pos);
  }

  // We create immediate_mode commands based on the nodes in our scene.
  // So we can reuse the capabilities of the immediate-command-processor
  // to create actual vulkan render commands.
  for (auto& node : nodes)
  {
    if (node.geometryType == PrimitiveGeometryType::Quad) {
      immediateCommandProcessor->renderQuad(node.transform);
    }
    else if (node.geometryType == PrimitiveGeometryType::Cube)
    {
      immediateCommandProcessor->renderCube(node.transform);
    }
  }

  auto& frameCommandBuffer = immediateCommandProcessor->recordFrameCommandBuffer();
  return frameCommandBuffer;

}

void tz::Scene::render()
{

  immediateCommandProcessor->recordAndSubmitFrameCommandBuffer();
  framePrimitives.clear();

}


