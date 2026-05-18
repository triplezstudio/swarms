#include <render.hh>
#include <window_system.hh>
#include <vulkan_renderer.hh>
#include <Eigen/Dense>
#include <text_render.hh>
#include "drawing.hh"

namespace rv = tz::render::vulkan;

static void createMasterPipelineLayout();
static void prepareRenderPrimitives();
static void buildPSOCache();

static tz::render::vulkan::Renderer* renderer = nullptr;
static tz::text::TextRenderer* textRenderer = nullptr;
static rv::DescriptorSet* cameraDescriptorSet = nullptr;
static rv::DescriptorSet* perObjectDescriptorSet = nullptr;
static rv::DescriptorSet* diffuseTextureDescriptorSet = nullptr;
static rv::PipelineLayout* masterPipelineLayout = nullptr;
static std::vector<uint32_t> quadIndices;
static std::vector<uint32_t> cubeIndices;
static std::vector<uint32_t> cubeIndicesPosTex;
static rv::Buffer* quadPosVertexBuffer       = nullptr;
static rv::Buffer* cubePosVertexBuffer = nullptr;
static rv::Buffer* cubePosTexCoordVertexBuffer = nullptr;
static rv::Buffer* quadPosTexCoordVertexBuffer = nullptr;
static rv::Buffer* quadIndexBuffer = nullptr;
static rv::Buffer* cubeIndexBuffer = nullptr;
static rv::Buffer* cubeTexIndexBuffer = nullptr;
static rv::Buffer* tzLabelVertexBuffer = nullptr;
static rv::Buffer* tzLabelIndexBuffer = nullptr;
//static std::map<std::string, tz::text::TextGeometry> textGeometries;
static std::map<std::string, rv::Buffer*> textVertexBuffers;
static std::map<std::string, rv::Buffer*> textIndexBuffers;
static std::map<int, uint32_t> fontTextureMap;
static uint32_t tzLabelIndexCount = 0;
static rv::PipelineStateObject* colorOnlyPSO = nullptr;
static rv::CommandBuffer* commandBuffer = nullptr;
static std::vector<tz::PrimitiveRenderData> framePrimitives;
static std::unordered_map<uint64_t, rv::PipelineStateObject*> psoCache;
static int uiFont = -1;
static uint32_t globalTextureIndex = 0;

static tz::Camera * default3DCamera = nullptr;
static tz::Camera * defaultUICamera = nullptr;

struct alignas(16) PerObjectUniformBufferObject
{
  Eigen::Matrix4f model;
  uint32_t textureId;
  uint32_t padding[3];

};

void TZ_API initRenderer(tz::Window* window)
{
  renderer = new tz::render::vulkan::Renderer();
  renderer->init(window);
  createMasterPipelineLayout();
  prepareRenderPrimitives();
  buildPSOCache();
  commandBuffer = renderer->createCommandBuffer();

  default3DCamera = new tz::Camera({0, 2, 3}, {0, 0, 0 }, tz::CameraType::Perspective);
  defaultUICamera = new tz::Camera({0, 0, 1}, {0, 0, 0}, tz::CameraType::Ortho);
}



static rv::PipelineStateObject* createColorOnlyPSO()
{


  auto vsPath = "shader_binaries/default_colored_transform_vs.slang.spv";
  auto fsPath = "shader_binaries/default_colored_transform_fs.slang.spv";

  auto vs = renderer->createShaderModule(rv::ShaderType::Vertex, vsPath);
  auto fs = renderer->createShaderModule(rv::ShaderType::Fragment, fsPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = rv::RenderState {};
  renderState.primitiveType = rv::PrimitiveType::Triangles;
  renderState.blending = false;
  renderState.depthTesting = true;
  renderState.cullMode = vk::CullModeFlagBits::eBack;
  renderState.fillMode = vk::PolygonMode::eFill;
  renderState.frontFace = vk::FrontFace::eCounterClockwise;
  renderState.stencilTesting = false;
  shaderPipeline = shaderPipeline;

  auto vertexLayout = rv::VertexLayout {};
  vertexLayout.bindings =  {rv::VertexBinding {
      .bufferSlot = 0,
      .stride = sizeof(rv::VertexPos),
      .vertexInputRate=rv::VertexInputRate::PerVertex,
  }};
  vertexLayout.attributes = {
    rv::VertexAttribute {
      .shaderLocation = 0,
      .bufferSlot = 0,
      .dataType = rv::DataType::Float,
      .componentCount = 3,
      .offset = 0
    }
  };

  // Descriptor layout and binding for the transformation matrix



  auto pso = renderer->createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                              vertexLayout,
                                                 masterPipelineLayout);

  return pso;
}

static rv::PipelineStateObject* createTextPSO()
{
  auto spvVertexShaderPath = "shader_binaries/default_textured_vs.slang.spv";
  auto spvFragmentShaderPath = "shader_binaries/text_fs.slang.spv";

  auto vs = renderer->createShaderModule(rv::ShaderType::Vertex, spvVertexShaderPath);
  auto fs = renderer->createShaderModule(rv::ShaderType::Fragment, spvFragmentShaderPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = rv::RenderState {};
  renderState.primitiveType = rv::PrimitiveType::Triangles;
  renderState.blending = true;
  renderState.depthTesting = true;
  renderState.cullMode = vk::CullModeFlagBits::eBack;
  renderState.fillMode = vk::PolygonMode::eFill;
  renderState.frontFace = vk::FrontFace::eCounterClockwise;
  renderState.stencilTesting = false;
  shaderPipeline = shaderPipeline;

  auto vertexLayout = rv::VertexLayout {};
  vertexLayout.bindings =  {rv::VertexBinding {
      .bufferSlot = 0,
      .stride = sizeof(rv::VertexPosTexCoords),
      .vertexInputRate=rv::VertexInputRate::PerVertex,
  }};
  vertexLayout.attributes = {
    rv::VertexAttribute {
      .shaderLocation = 0,
      .bufferSlot = 0,
      .dataType = rv::DataType::Float,
      .componentCount = 3,
      .offset = 0
    },
    {
      rv::VertexAttribute
      {
        .shaderLocation = 1,
        .bufferSlot = 0,
        .dataType = rv::DataType::Float,
        .componentCount = 2,
        .offset = sizeof(float) * 3
      }
    }
  };


  auto pso = renderer->createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                                 vertexLayout,
                                                 masterPipelineLayout);
  return pso;
}


static rv::PipelineStateObject* createTexturedPSO()
{

  auto vsPath = "shader_binaries/default_textured_vs.slang.spv";
  auto fsPath = "shader_binaries/default_textured_fs.slang.spv";

  auto vs = renderer->createShaderModule(rv::ShaderType::Vertex, vsPath);
  auto fs = renderer->createShaderModule(rv::ShaderType::Fragment, fsPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = rv::RenderState {};
  renderState.primitiveType = rv::PrimitiveType::Triangles;
  renderState.blending = true;
  renderState.depthTesting = true;
  renderState.cullMode = vk::CullModeFlagBits::eBack;
  renderState.fillMode = vk::PolygonMode::eFill;
  renderState.frontFace = vk::FrontFace::eCounterClockwise;
  renderState.stencilTesting = false;
  shaderPipeline = shaderPipeline;

  auto vertexLayout = rv::VertexLayout {};
  vertexLayout.bindings =  {rv::VertexBinding {
    .bufferSlot = 0,
    .stride = sizeof(rv::VertexPosTexCoords),
    .vertexInputRate=rv::VertexInputRate::PerVertex,
}};
  vertexLayout.attributes = {
    rv::VertexAttribute {
      .shaderLocation = 0,
      .bufferSlot = 0,
      .dataType = rv::DataType::Float,
      .componentCount = 3,
      .offset = 0
    },
    {
      rv::VertexAttribute
      {
        .shaderLocation = 1,
        .bufferSlot = 0,
        .dataType = rv::DataType::Float,
        .componentCount = 2,
        .offset = sizeof(float) * 3
      }
    }
  };


  auto pso = renderer->createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                                 vertexLayout,
                                                 masterPipelineLayout);
  return pso;
}


/**
 * Here we create all needed PSO variants.
 * We store them in a map which understands
 * RenderingHints hashed keys.
 */
static void buildPSOCache()
{
  using namespace tz;
  colorOnlyPSO = createColorOnlyPSO();
  tz::RenderHints colorOnlyHints;
  colorOnlyHints.materialType = tz::MaterialType::SingleColor;
  colorOnlyHints.vertexShaderType = VertexShaderType::Static;
  psoCache[colorOnlyHints.getHash()] = colorOnlyPSO;

  auto texturedPSO = createTexturedPSO();
  tz::RenderHints texturedHints;
  texturedHints.materialType = MaterialType::DiffuseNormal;
  texturedHints.vertexShaderType = VertexShaderType::Static;
  psoCache[texturedHints.getHash()] = texturedPSO;

  auto textPSO = createTextPSO();
  tz::RenderHints textHints;
  textHints.materialType = MaterialType::Text;
  textHints.vertexShaderType = VertexShaderType::Static;
  psoCache[textHints.getHash()] = textPSO;

}

static void prepareRenderPrimitives()
{

  std::vector<rv::VertexPos> verticesPos =
    {
      {{-0.5, 0.5, 0.5}},
      {{-0.5, -0.5, 0.5}},
      {{0.5, -0.5, 0.5}},
      {{0.5, 0.5, 0.5}}
    };

  std::vector<rv::VertexPosTexCoords> verticesPosTexCoord =
    {
      {{-0.5, 0.5, 0.5}, {0, 1}},
      {{-0.5, -0.5, 0.5}, {0, 0}},
      {{0.5, -0.5, 0.5},  {1, 0}},
      {{0.5, 0.5, 0.5}, {1, 1}}
    };

  std::vector<rv::VertexPos> cubeVerticesPos = {
    {{-.5f, -.5f,  .50f}}, // 0: Front-Bottom-Left
    {{ .5f, -.5f,  .5f}}, // 1: Front-Bottom-Right
    {{ .5f,  .5f,  .5f}}, // 2: Front-Top-Right
    {{-.5f,  .5f,  .5f}}, // 3: Front-Top-Left
    {{-.5f, -.5f, -.5f}}, // 4: Back-Bottom-Left
    {{ .5f, -.5f, -.5f}}, // 5: Back-Bottom-Right
    {{ .5f,  .5f, -.5f}}, // 6: Back-Top-Right
    {{-.5f,  .5f, -.5f}}  // 7: Back-Top-Left
  };

  std::vector<rv::VertexPosTexCoords> cubeVerticesPosTex = {
    // Front face (Z = 1.0f)
    {{-.5f, -.5f,  .5f}, {0.0f, 0.0f}}, {{ .5f, -.5f,  .5f}, {1.0f, 0.0f}},
    {{ .5f,  .5f,  .5f}, {1.0f, 1.0f}}, {{-.5f,  .5f,  .5f}, {0.0f, 1.0f}},

    // Back face (Z = -1.0f)
    {{ .5f, -.5f, -.5f}, {0.0f, 0.0f}}, {{-.5f, -.5f, -.5f}, {1.0f, 0.0f}},
    {{-.5f,  .5f, -.5f}, {1.0f, 1.0f}}, {{ .5f,  .5f, -.5f}, {0.0f, 1.0f}},

    // Left face (X = -1.0f)
    {{-.5f, -.5f, -.5f}, {0.0f, 0.0f}}, {{-.5f, -.5f,  .5f}, {1.0f, 0.0f}},
    {{-.5f,  .5f,  .5f}, {1.0f, 1.0f}}, {{-.5f,  .5f, -.5f}, {0.0f, 1.0f}},

    // Right face (X = 1.0f)
    {{ .5f, -.5f,  .5f}, {0.0f, 0.0f}}, {{ .5f, -.5f, -.5f}, {1.0f, 0.0f}},
    {{ .5f,  .5f, -.5f}, {1.0f, 1.0f}}, {{ .5f,  .5f,  .5f}, {0.0f, 1.0f}},

    // Top face (Y = 1.0f)
    {{-.5f,  .5f,  .5f}, {0.0f, 0.0f}}, {{ .5f,  .5f,  .5f}, {1.0f, 0.0f}},
    {{ .5f,  .5f, -.5f}, {1.0f, 1.0f}}, {{-.5f,  .5f, -.5f}, {0.0f, 1.0f}},

    // Bottom face (Y = -1.0f)
    {{-.5f, -.5f, -.5f}, {0.0f, 0.0f}}, {{ .5f, -.5f, -.5f}, {1.0f, 0.0f}},
    {{ .5f, -.5f,  .5f}, {1.0f, 1.0f}}, {{-.5f, -.5f,  .5f}, {0.0f, 1.0f}}
  };


  quadIndices =
    {
      0, 1, 2,
      0, 2, 3
    };

  cubeIndices = {
    // Front face
    0, 1, 2,  2, 3, 0,
    // Right face
    1, 5, 6,  6, 2, 1,
    // Back face
    5, 4, 7,  7, 6, 5,
    // Left face
    4, 0, 3,  3, 7, 4,
    // Bottom face
    4, 5, 1,  1, 0, 4,
    // Top face
    3, 2, 6,  6, 7, 3
  };

  cubeIndicesPosTex = {
    0,  1,  2,   2,  3,  0, // Front
    4,  5,  6,   6,  7,  4, // Back
    8,  9, 10,  10, 11,  8, // Left
    12, 13, 14,  14, 15, 12, // Right
    16, 17, 18,  18, 19, 16, // Top
    20, 21, 22,  22, 23, 20  // Bottom
  };

  quadPosVertexBuffer = renderer->createBuffer(verticesPos.data(),
                                                 verticesPos.size() * sizeof (rv::VertexPos),
                                                 rv::BufferUsage::Vertex);

  cubePosVertexBuffer = renderer->createBuffer(cubeVerticesPos.data(),
                                               cubeVerticesPos.size() * sizeof (rv::VertexPos),
                                               rv::BufferUsage::Vertex);

  cubePosTexCoordVertexBuffer = renderer->createBuffer(cubeVerticesPosTex.data(),
                                                       cubeVerticesPosTex.size() * sizeof (rv::VertexPosTexCoords),
                                                       rv::BufferUsage::Vertex);


  quadPosTexCoordVertexBuffer = renderer->createBuffer(verticesPosTexCoord.data(),
                                            verticesPosTexCoord.size() * sizeof (rv::VertexPosTexCoords),
                                            rv::BufferUsage::Vertex);

  quadIndexBuffer = renderer->createBuffer(quadIndices.data(),
                                                quadIndices.size() * sizeof(uint32_t),
                                                rv::BufferUsage::Index);

  cubeIndexBuffer = renderer->createBuffer(cubeIndices.data(),
                                           cubeIndices.size() * sizeof(uint32_t),
                                           rv::BufferUsage::Index);

  cubeTexIndexBuffer = renderer->createBuffer(cubeIndicesPosTex.data(),
                                           cubeIndicesPosTex.size() * sizeof(uint32_t),
                                           rv::BufferUsage::Index);



    textRenderer = new tz::text::TextRenderer(*renderer);
    uiFont = textRenderer->createFont("assets/consola.ttf", 12);
    renderer->updateTextureDescriptorSet(diffuseTextureDescriptorSet, 0, globalTextureIndex++, textRenderer->getAtlasTextureForFont(uiFont));
}

static std::vector<tz::PrimitiveRenderData> getRenderPrimitivesByCamera(tz::Camera* camera)
{
  std::vector<tz::PrimitiveRenderData> filteredPrimitiveData;
  for (auto& rp : framePrimitives)
  {
    if (rp.associatedCamera == camera)
    {
      filteredPrimitiveData.push_back(rp);
    }
  }

  return filteredPrimitiveData;

}

static void renderPrimitives(const std::vector<tz::PrimitiveRenderData>& primitives, uint32_t& primitiveCounter)
{
  for (auto& prd : primitives)
  {
    auto pso = psoCache[prd.renderHints.getHash()];
    renderer->recordCommand(commandBuffer,new rv::CmdBindPipeline (pso));

    auto transform = Eigen::Affine3f::Identity();
    transform.translate(prd.transform.position);
    transform.scale(prd.transform.scale);
    Eigen::Matrix4f tm = transform.matrix();
    PerObjectUniformBufferObject perObjectUBO;
    perObjectUBO.model = tm;
    perObjectUBO.textureId = prd.renderHints.texture;
    renderer->updateBuffer(perObjectDescriptorSet->layout->descriptorBindings[0]->buffer, &perObjectUBO, sizeof(PerObjectUniformBufferObject),
                           primitiveCounter);
    renderer->recordCommand(commandBuffer, new rv::CmdBindDescriptors({perObjectDescriptorSet}, masterPipelineLayout, {primitiveCounter}, 1));

    renderer->recordCommand(commandBuffer, new rv::CmdSetViewPorts({{0, 0, 640, 480}}));
    renderer->recordCommand(commandBuffer, new rv::CmdSetScissors({{0, 0, 640, 480}}));

    renderer->recordCommand(commandBuffer,new rv::CmdBindVertexBuffers({prd.vertexBuffer}));
    renderer->recordCommand(commandBuffer, new rv::CmdBindIndexBuffer(prd.indexBuffer, 0));
    renderer->recordCommand(commandBuffer, new rv::CmdDrawIndexed(prd.indexCount, 1,0, 0, 0));

    primitiveCounter++;
  }
}


void TZ_API renderFrame()
{


  renderer->beginFrame();
  renderer->beginCommandBuffer(commandBuffer);

  // We can bind our diffuseTextureDescriptorSet once at the beginning of the frame.
  // This descriptorSet contains slots for up to 1000 textures.
  // The actual texture is then just indexed by the individual rendered object (see renderPrimitives)
  renderer->recordCommand(commandBuffer, new rv::CmdBindDescriptors({diffuseTextureDescriptorSet}, masterPipelineLayout, {0}, 2));

  // We are rendering the scene ordered by "camera".
  // First everything which has the 3d scene camera,
  // then the 2d ui camera.
  // This is more efficient in terms of pipeline-binding and alos
  // makes sure, UI always renders on top of everything else.
  auto camera3DPrimitives = getRenderPrimitivesByCamera(default3DCamera);
  tz::CameraUniformBufferObject cameraUBO;
  cameraUBO.view = default3DCamera->lookAtRH();
  cameraUBO.proj = default3DCamera->getProjectionMatrix(640, 480);
  renderer->recordCommand(commandBuffer, new rv::CmdBindDescriptors({cameraDescriptorSet}, masterPipelineLayout, {0}, 0));
  renderer->updateBuffer(cameraDescriptorSet->layout->descriptorBindings[0]->buffer, &cameraUBO, sizeof(tz::CameraUniformBufferObject),
                         0);

  uint32_t primitiveCounter = 0;
  renderPrimitives(camera3DPrimitives, primitiveCounter);

  // Next UI:
  auto cameraUIPrimitives = getRenderPrimitivesByCamera(defaultUICamera);
  cameraUBO.view = defaultUICamera->lookAtRH();
  cameraUBO.proj = defaultUICamera->getProjectionMatrix(640, 480);
  renderer->recordCommand(commandBuffer, new rv::CmdBindDescriptors({cameraDescriptorSet}, masterPipelineLayout, {1}, 0));

  renderer->updateBuffer(cameraDescriptorSet->layout->descriptorBindings[0]->buffer, &cameraUBO,
                         sizeof(tz::CameraUniformBufferObject),
                         1);
  renderPrimitives(cameraUIPrimitives, primitiveCounter);

  renderer->endCommandBuffer(commandBuffer);
  renderer->submitCommandBuffer(commandBuffer);
  renderer->endFrame();

  framePrimitives.clear();
}



static void createMasterPipelineLayout()
{
  // Camera is set0, binding0
  auto cameraBuffer = renderer->createMultiframeUniformBuffer(2,
    sizeof(tz::CameraUniformBufferObject));
  auto cameraUBOBinding = renderer->createDescriptorBinding(0, rv::DescriptorResourceType::Ubo,
                                                              rv::ShaderType::Vertex, 1,
                                                              cameraBuffer);
  auto cameraDescriptorSetLayout =  (renderer->createDescriptorSetLayout({cameraUBOBinding}));
  cameraDescriptorSet = renderer->createMultiframeDescriptorSet(cameraDescriptorSetLayout);

  // PerObject is set1, binding0
  auto perObjectBuffer = renderer->createMultiframeUniformBuffer(10000,
    sizeof(PerObjectUniformBufferObject));
  auto perObjectUBOBinding = renderer->createDescriptorBinding(0, rv::DescriptorResourceType::Ubo,
                                                               rv::ShaderType::Vertex, 1,
                                                               perObjectBuffer);
  auto perObjectDescriptorSetLayout = renderer->createDescriptorSetLayout({perObjectUBOBinding});
  perObjectDescriptorSet = renderer->createMultiframeDescriptorSet(perObjectDescriptorSetLayout);

  // Diffuse textures at set2, binding0.
  // We allow up to 1000 textures
  auto textureDescBinding = renderer->createDescriptorBinding(0, rv::DescriptorResourceType::Sampler,
                                                              rv::ShaderType::Fragment, 1000, nullptr, nullptr);

  auto diffuseTextureDescriptorSetLayout = renderer->createDescriptorSetLayout({textureDescBinding}, true);
  diffuseTextureDescriptorSet = renderer->createMultiframeDescriptorSet(diffuseTextureDescriptorSetLayout);

  masterPipelineLayout = renderer->createPipelineLayout({cameraDescriptorSetLayout, perObjectDescriptorSetLayout, diffuseTextureDescriptorSetLayout});


}
