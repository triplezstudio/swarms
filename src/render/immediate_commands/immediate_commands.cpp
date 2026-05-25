#include <immediate_commands.hh>
#include <render_helpers.hh>

void tz::ImmediateCommandProcessor::renderCube(tz::Transform transform, tz::RenderHints renderHints)
{
  PrimitiveRenderData prd;
  prd.transform = transform;
  prd.geometryType = PrimitiveGeometryType::Cube;
  prd.renderHints = renderHints;
  prd.associatedCamera = activeRenderCamera;
  prd.vertexBuffer = renderHints.materialType == rv::MaterialType::SingleColor ? cubePosVertexBuffer : cubePosTexCoordVertexBuffer;
  prd.indexBuffer = renderHints.materialType == rv::MaterialType::SingleColor ? cubeIndexBuffer : cubeTexIndexBuffer;
  prd.indexCount = renderHints.materialType == rv::MaterialType::SingleColor ? getCubeIndices().size() : getCubeIndicesPosTex().size();
  framePrimitives.push_back(prd);
  
}


void tz::ImmediateCommandProcessor::renderText(const std::string& text, tz::render::Font& font,
                                               Transform transform,
                                               Eigen::Vector4f color)
{
  RenderHints textRenderHints;
  textRenderHints.materialType = rv::MaterialType::Text;
  textRenderHints.vertexShaderType = rv::VertexShaderType::Static;
  textRenderHints.texture = font.textureId;
  textRenderHints.color = color;
  tz::PrimitiveRenderData prd;
  prd.transform = transform;;
  prd.geometryType     = PrimitiveGeometryType::Quad;
  prd.renderHints = textRenderHints;
  prd.associatedCamera = activeRenderCamera;


  if (textVertexBuffers.find(text) == textVertexBuffers.end())
  {
    auto textGeometry = textRenderer.createGeometryForText(text, font);
    textGeometries[text]= textGeometry;

    std::vector<rv::VertexPosTexCoords> vertices;
    for (int i = 0; i < textGeometry.positions.size();i++)
    {
      rv::VertexPosTexCoords vertex;
      vertex.pos = textGeometry.positions[i];
      vertex.texCoords = textGeometry.texCoords[i];
      vertices.push_back(vertex);
    }

    textVertexBuffers[text] = renderer.createBuffer(vertices.data(),
                                                     vertices.size() * sizeof (rv::VertexPosTexCoords),
                                                     rv::BufferUsage::Vertex);

    textIndexBuffers[text] = renderer.createBuffer(textGeometry.indices.data(),
                                                    textGeometry.indices.size() * sizeof(uint32_t),
                                                    rv::BufferUsage::Index);
  }

  prd.vertexBuffer = textVertexBuffers[text];
  prd.indexBuffer = textIndexBuffers[text];
  prd.indexCount = textGeometries[text].indices.size();
  framePrimitives.push_back(prd);

}

void tz::ImmediateCommandProcessor::renderQuad(Transform transform, RenderHints renderHints)
{
  PrimitiveRenderData prd;
  prd.transform = transform;;
  prd.geometryType     = PrimitiveGeometryType::Quad;
  prd.renderHints = renderHints;
  prd.associatedCamera = activeRenderCamera;
  prd.vertexBuffer = renderHints.materialType == rv::MaterialType::SingleColor ? quadPosVertexBuffer : quadPosTexCoordVertexBuffer;
  prd.indexBuffer = quadIndexBuffer;
  prd.indexCount = getQuadIndices().size();
  framePrimitives.push_back(prd);
}

void tz::ImmediateCommandProcessor::activate3DCamera(Eigen::Vector3f position, Eigen::Vector3f lookAt)
{
  default3DCamera->pos = position;
  default3DCamera->lookAt = lookAt;
  activeRenderCamera = default3DCamera;
}
void tz::ImmediateCommandProcessor::activate3DCamera()
{
  activeRenderCamera = default3DCamera;
}
void tz::ImmediateCommandProcessor::activateUICamera()
{
  activeRenderCamera = defaultUICamera;
}
void tz::ImmediateCommandProcessor::activateUICamera(Eigen::Vector3f position)
{
  activeRenderCamera = defaultUICamera;
  defaultUICamera->pos = position;
  defaultUICamera->lookAt = Eigen::Vector3f (position.x(), position.y(), -position.z());
}
void tz::ImmediateCommandProcessor::renderSphere(Transform transform, RenderHints renderHints)
{
  throw std::runtime_error("not yet implemented: renderSphere!");
}

void tz::ImmediateCommandProcessor::renderCylinder(Transform transform, RenderHints renderHints)
{
  throw std::runtime_error("not yet implemented: renderCylinder!");
}
tz::ImmediateCommandProcessor::ImmediateCommandProcessor(tz::render::vulkan::Renderer& renderer,
                                                         tz::render::TextRenderer& textRenderer,
                                                         MasterPipelineLayout& masterPipelineLayout)
    : renderer(renderer), textRenderer(textRenderer), masterPipelineLayout(masterPipelineLayout)
{

  buildPSOCache();
  commandBuffer = renderer.createCommandBuffer();

  default3DCamera = new Camera({0, 2, 3}, {0, 0, 0 }, CameraType::Perspective);
  defaultUICamera = new Camera({0, 0, 1}, {0, 0, 0}, CameraType::Ortho);

  {
    std::vector<rv::VertexPos> verticesPos;
    fillWithQuadVertices(verticesPos);

    std::vector<rv::VertexPosTexCoords> verticesPosTexCoord;
    fillWithQuadVertices(verticesPosTexCoord);

    std::vector<rv::VertexPos> cubeVerticesPos;
    fillWithCubeVertices(cubeVerticesPos);

    std::vector<rv::VertexPosTexCoords> cubeVerticesPosTex;
    fillWithCubeVertices(cubeVerticesPosTex);




    quadPosVertexBuffer = renderer.createBuffer(verticesPos.data(),
                                                 verticesPos.size() * sizeof (rv::VertexPos),
                                                 rv::BufferUsage::Vertex);

    cubePosVertexBuffer = renderer.createBuffer(cubeVerticesPos.data(),
                                                 cubeVerticesPos.size() * sizeof (rv::VertexPos),
                                                 rv::BufferUsage::Vertex);

    cubePosTexCoordVertexBuffer = renderer.createBuffer(cubeVerticesPosTex.data(),
                                                         cubeVerticesPosTex.size() * sizeof (rv::VertexPosTexCoords),
                                                         rv::BufferUsage::Vertex);


    quadPosTexCoordVertexBuffer = renderer.createBuffer(verticesPosTexCoord.data(),
                                                         verticesPosTexCoord.size() * sizeof (rv::VertexPosTexCoords),
                                                         rv::BufferUsage::Vertex);

    quadIndexBuffer = renderer.createBuffer(getQuadIndices().data(),
                                             getQuadIndices().size() * sizeof(uint32_t),
                                             rv::BufferUsage::Index);

    cubeIndexBuffer = renderer.createBuffer(getCubeIndices().data(),
                                             getCubeIndices().size() * sizeof(uint32_t),
                                             rv::BufferUsage::Index);

    cubeTexIndexBuffer = renderer.createBuffer(getCubeIndicesPosTex().data(),
                                                getCubeIndicesPosTex().size() * sizeof(uint32_t),
                                                rv::BufferUsage::Index);
  }
}

std::vector<tz::PrimitiveRenderData> tz::ImmediateCommandProcessor::getRenderPrimitivesByCamera(Camera* camera)
{
  std::vector<PrimitiveRenderData> filteredPrimitiveData;
  for (auto& rp : framePrimitives)
  {
    if (rp.associatedCamera == camera)
    {
      filteredPrimitiveData.push_back(rp);
    }
  }

  return filteredPrimitiveData;

}


tz::render::vulkan::PipelineStateObject* tz::ImmediateCommandProcessor::createColorOnlyPSO()
{
  
  auto vsPath = "shader_binaries/default_colored_transform_vs.slang.spv";
  auto fsPath = "shader_binaries/default_colored_transform_fs.slang.spv";

  auto vs = renderer.createShaderModule(rv::ShaderType::Vertex, vsPath);
  auto fs = renderer.createShaderModule(rv::ShaderType::Fragment, fsPath);
  auto shaderPipeline = renderer.createShaderPipeline({vs, fs});

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



  auto pso = renderer.createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                                 vertexLayout,
                                                 masterPipelineLayout.getPipelineLayoutPtr());

  return pso;
}

tz::render::vulkan::PipelineStateObject* tz::ImmediateCommandProcessor::createTextPSO()
{
  auto spvVertexShaderPath = "shader_binaries/default_textured_vs.slang.spv";
  auto spvFragmentShaderPath = "shader_binaries/text_fs.slang.spv";

  auto vs = renderer.createShaderModule(rv::ShaderType::Vertex, spvVertexShaderPath);
  auto fs = renderer.createShaderModule(rv::ShaderType::Fragment, spvFragmentShaderPath);
  auto shaderPipeline = renderer.createShaderPipeline({vs, fs});

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


  auto pso = renderer.createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                                 vertexLayout,
                                                 masterPipelineLayout.getPipelineLayoutPtr());
  return pso;
}


tz::render::vulkan::PipelineStateObject* tz::ImmediateCommandProcessor::createTexturedPSO()
{

  auto vsPath = "shader_binaries/default_textured_vs.slang.spv";
  auto fsPath = "shader_binaries/default_textured_fs.slang.spv";

  auto vs = renderer.createShaderModule(rv::ShaderType::Vertex, vsPath);
  auto fs = renderer.createShaderModule(rv::ShaderType::Fragment, fsPath);
  auto shaderPipeline = renderer.createShaderPipeline({vs, fs});

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


  auto pso = renderer.createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                                 vertexLayout,
                                                 masterPipelineLayout.getPipelineLayoutPtr());
  return pso;
}


/**
 * Here we create all needed PSO variants.
 * We store them in a map which understands
 * RenderingHints hashed keys.
 */
void tz::ImmediateCommandProcessor::buildPSOCache()
{
  colorOnlyPSO = createColorOnlyPSO();
  RenderHints colorOnlyHints;
  colorOnlyHints.materialType = rv::MaterialType::SingleColor;
  colorOnlyHints.vertexShaderType = rv::VertexShaderType::Static;
  psoCache.put(colorOnlyHints.toCacheKey(), colorOnlyPSO);
  
  auto texturedPSO = createTexturedPSO();
  RenderHints texturedHints;
  texturedHints.materialType = rv::MaterialType::DiffuseNormal;
  texturedHints.vertexShaderType = rv::VertexShaderType::Static;
  psoCache.put(texturedHints.toCacheKey(), texturedPSO);

  auto textPSO = createTextPSO();
  RenderHints textHints;
  textHints.materialType = rv::MaterialType::Text;
  textHints.vertexShaderType = rv::VertexShaderType::Static;
  psoCache.put(textHints.toCacheKey(), textPSO);

}


void tz::ImmediateCommandProcessor::renderPrimitives(const std::vector<PrimitiveRenderData>& primitives, uint32_t& primitiveCounter)
{

  for (auto& prd : primitives)
  {
    auto pso = psoCache.get(prd.renderHints.toCacheKey());
    renderer.recordCommand(commandBuffer,new rv::CmdBindPipeline (pso));

    auto transform = Eigen::Affine3f::Identity();
    transform.translate(prd.transform.position);
    transform.scale(prd.transform.scale);
    Eigen::Matrix4f tm = transform.matrix();
    rv::PerObjectUniformBufferObject perObjectUBO;
    perObjectUBO.model = tm;
    perObjectUBO.textureId = prd.renderHints.texture;
    perObjectUBO.color = prd.renderHints.color;
    renderer.updateBuffer(masterPipelineLayout.getPerObjectDescriptorSetPtr()->layout->descriptorBindings[0]->buffer, &perObjectUBO, sizeof(rv::PerObjectUniformBufferObject),
                           primitiveCounter);
    renderer.recordCommand(commandBuffer, new rv::CmdBindDescriptors({masterPipelineLayout.getPerObjectDescriptorSetPtr()}, masterPipelineLayout.getPipelineLayoutPtr(), {primitiveCounter}, 1));

    renderer.recordCommand(commandBuffer, new rv::CmdSetViewPorts({{0, 0, 640, 480}}));
    renderer.recordCommand(commandBuffer, new rv::CmdSetScissors({{0, 0, 640, 480}}));

    renderer.recordCommand(commandBuffer,new rv::CmdBindVertexBuffers({prd.vertexBuffer}));
    renderer.recordCommand(commandBuffer, new rv::CmdBindIndexBuffer(prd.indexBuffer, 0));
    renderer.recordCommand(commandBuffer, new rv::CmdDrawIndexed(prd.indexCount, 1,0, 0, 0));

    primitiveCounter++;
  }
}

/**
 * Records the commands for the current frame into the commandbuffer
 * and returns a reference to it.
 * @return
 */
tz::render::vulkan::CommandBuffer& tz::ImmediateCommandProcessor::recordFrameCommandBuffer()
{
  renderer.beginCommandBuffer(commandBuffer);

  // We can bind our diffuseTextureDescriptorSet once at the beginning of the frame.
  // This descriptorSet contains slots for up to 1000 textures.
  // The actual texture is then just indexed by the individual rendered object (see renderPrimitives)
  renderer.recordCommand(commandBuffer, new rv::CmdBindDescriptors({masterPipelineLayout.getDiffuseTextureDescriptorSetPtr()}, masterPipelineLayout.getPipelineLayoutPtr(), {0}, 2));


  // We are rendering the scene ordered by "camera".
  // First everything which has the 3d scene camera,
  // then the 2d ui camera.
  // This is more efficient in terms of pipeline-binding and alos
  // makes sure, UI always renders on top of everything else.
  auto camera3DPrimitives = getRenderPrimitivesByCamera(default3DCamera);
  rv::CameraUniformBufferObject cameraUBO;
  cameraUBO.view = default3DCamera->lookAtRH();
  cameraUBO.proj = default3DCamera->getProjectionMatrix(640, 480);
  renderer.recordCommand(commandBuffer, new rv::CmdBindDescriptors({masterPipelineLayout.getCameraDescriptorSetPtr()}, masterPipelineLayout.getPipelineLayoutPtr(), {0}, 0));
  renderer.updateBuffer(masterPipelineLayout.getCameraDescriptorSetPtr()->layout->descriptorBindings[0]->buffer, &cameraUBO, sizeof(rv::CameraUniformBufferObject),
                        0);

  uint32_t primitiveCounter = 0;
  renderPrimitives(camera3DPrimitives, primitiveCounter);

  // Next UI:
  auto cameraUIPrimitives = getRenderPrimitivesByCamera(defaultUICamera);
  cameraUBO.view = defaultUICamera->lookAtRH();
  cameraUBO.proj = defaultUICamera->getProjectionMatrix(640, 480);
  renderer.recordCommand(commandBuffer, new rv::CmdBindDescriptors({masterPipelineLayout.getCameraDescriptorSetPtr()}, masterPipelineLayout.getPipelineLayoutPtr(), {1}, 0));

  renderer.updateBuffer(masterPipelineLayout.getCameraDescriptorSetPtr()->layout->descriptorBindings[0]->buffer, &cameraUBO,
                        sizeof(rv::CameraUniformBufferObject),
                        1);
  renderPrimitives(cameraUIPrimitives, primitiveCounter);

  renderer.endCommandBuffer(commandBuffer);

  framePrimitives.clear();

  return *commandBuffer;

}
void tz::ImmediateCommandProcessor::recordAndSubmitFrameCommandBuffer()
{
  recordFrameCommandBuffer();
  renderer.submitCommandBuffer(commandBuffer);

}
