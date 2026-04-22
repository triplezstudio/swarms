
#include <Eigen/Dense>
#include <render.hh>
#include <opengl_renderer.hh>
#ifdef USE_VULKAN
#include <vulkan_renderer.hh>
#endif
#include <sdl2.hh>
#include <cmath>

/**
 * Helper functions for creating some PipelineStateObjects with different render states and vertex layouts.
 */
tz::PipelineStateObject* createTexturedPSO(tz::Renderer* renderer)
{

  auto spvPath = "shader_binaries/default_shader_stage4.slang.spv";

  auto vs = renderer->createShaderModule(tz::ShaderType::Vertex, spvPath);
  auto fs = renderer->createShaderModule(tz::ShaderType::Fragment, spvPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = tz::RenderState {};
  renderState.primitiveType = tz::PrimitiveType::Triangles;
  renderState.cullMode = tz::CullMode::Back;
  renderState.blending = false;
  renderState.depthTesting = true;
  renderState.fillMode = tz::FillMode::Solid;
  renderState.frontFace = tz::FrontFace::CounterClockwise;
  renderState.stencilTesting = false;
  shaderPipeline = shaderPipeline;

  auto vertexLayout = tz::VertexLayout {};
  vertexLayout.bindings =  {tz::VertexBinding {
      .bufferSlot = 0,
      .stride = sizeof(tz::VertexPosTexCoords),
      .vertexInputRate=tz::VertexInputRate::PerVertex,
  }};
  vertexLayout.attributes = {
    tz::VertexAttribute {
      .shaderLocation = 0,
      .bufferSlot = 0,
      .dataType = tz::DataType::Float,
      .componentCount = 3,
      .offset = 0
    },
    {
      tz::VertexAttribute
      {
        .shaderLocation = 1,
        .bufferSlot = 0,
        .dataType = tz::DataType::Float,
        .componentCount = 2,
        .offset = sizeof(float) * 2
      }
    }
  };

  auto textureBitmapData = tz::loadBitmapDataFromPath("assets/test_image.png");
  auto textureImage = renderer->createImage(textureBitmapData);
  auto sampler = renderer->createSampler();
  auto textureImageView = renderer->createImageView(textureImage);
  auto texture = renderer->createTexture(textureImage);

  // Descriptor layout and binding for the transformation matrix:
  auto transformBuffer = renderer->createMultiframeBuffer(nullptr,
                                                          sizeof(tz::TransformUniformBufferObject),
                                                          tz::BufferUsage::Uniform);

  auto transformDescBinding = renderer->createDescriptorBinding(0,
                                                                tz::ResourceType::Ubo,
                                                                tz::ShaderType::Vertex,
                                                                1,
                                                                transformBuffer);
  auto textureDescBinding = renderer->createDescriptorBinding(1, tz::ResourceType::Sampler,
                                                              tz::ShaderType::Fragment, 1, nullptr, texture);

  auto descriptorSetLayout = renderer->createDescriptorSetLayout({transformDescBinding, textureDescBinding});
  auto descriptorSet = renderer->createMultiframeDescriptorSet(descriptorSetLayout);

  auto pso = renderer->createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                                 vertexLayout,
                                                            {descriptorSetLayout});
  pso->descriptorSets = {descriptorSet};
  return pso;
}

 tz::PipelineStateObject* createColorOnlyPSO(tz::Renderer* renderer)
 {

  auto spvPath = "shader_binaries/default_shader_stage3.slang.spv";

  auto vs = renderer->createShaderModule(tz::ShaderType::Vertex, spvPath);
  auto fs = renderer->createShaderModule(tz::ShaderType::Fragment, spvPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = tz::RenderState {};
  renderState.primitiveType = tz::PrimitiveType::Triangles;
  renderState.cullMode = tz::CullMode::Back;
  renderState.blending = false;
  renderState.depthTesting = true;
  renderState.fillMode = tz::FillMode::Solid;
  renderState.frontFace = tz::FrontFace::CounterClockwise;
  renderState.stencilTesting = false;
  shaderPipeline = shaderPipeline;
  
  auto vertexLayout = tz::VertexLayout {}; 
  vertexLayout.bindings =  {tz::VertexBinding {
      .bufferSlot = 0,
      .stride = sizeof(tz::VertexPosColor),
      .vertexInputRate=tz::VertexInputRate::PerVertex,
    }};
  vertexLayout.attributes = {
                        tz::VertexAttribute {
                          .shaderLocation = 0,
                          .bufferSlot = 0,
                          .dataType = tz::DataType::Float,
                          .componentCount = 3,
                          .offset = 0
                      },
                      {
                        tz::VertexAttribute
                        {
                          .shaderLocation = 1,
                          .bufferSlot = 0,
                          .dataType = tz::DataType::Float,
                          .componentCount = 3,
                          .offset = sizeof(float) * 3
                        }
                      }
                    };

  // Descriptor layout and binding for the transformation matrix:
  auto transformBuffer = renderer->createMultiframeBuffer(nullptr,
                                                          sizeof(tz::TransformUniformBufferObject),
                                                          tz::BufferUsage::Uniform);
  auto transformDescBinding = renderer->createDescriptorBinding(0,
                                                                tz::ResourceType::Ubo,
                                                                tz::ShaderType::Vertex,
                                                                1,
                                                                transformBuffer);
  auto descriptorSetLayout = renderer->createDescriptorSetLayout({transformDescBinding});
  auto descriptorSet = renderer->createMultiframeDescriptorSet(descriptorSetLayout);

  auto pso = renderer->createPipelineStateObject(renderState,
                                                             shaderPipeline,
                                                          vertexLayout,
                                                            {descriptorSetLayout});
  pso->descriptorSets = {descriptorSet};
  return pso;
 }


/**
 * This represents all the graphical resources we need for our game client:
 * - shaders
 * - textures
 * - pipeline-states
 * - geometry
 */
struct GameGraphicsData
{

  // We collect information related to
  // the rendering of our client data
  // in a custom struct which holds our
  // shaders, PSO, uniforms matrices, textures etc.
  GameGraphicsData(tz::Renderer* renderer, tz::ShaderPipeline* shaderPipeline)
  {

    // The PipelineStateObject holds all the state
    // we need for a pipeline execution, i.e. a draw call.
    // Think of renderstate handling and description
    // of vertex attributes and uniform buffers etc.
    // We must pre-create PSOs for all the different rendering configurations we need in our game.
    // This pre-generation makes it more verbose upfront,
    // but it allows us to avoid doing expensive state validation and setup during the game loop.
    // It is advisble to order meshes by PSO to minimize state changes during rendering.
    // In this demo setup we will create a few PSOs to accommodate a few different rendering 
    // configurations.
    auto defaultPSO = createColorOnlyPSO(renderer);
      
      
      tz::DescriptorBinding transformBinding;
      transformBinding.setIndex      = 0;
      transformBinding.bindingIndex  = 1;
      transformBinding.type = tz::ResourceType::Ubo;
      defaultPSO->descriptorBindings = {
        &transformBinding
      };

      auto transform = Eigen::Affine3f::Identity();
      transform.translate(Eigen::Vector3f(-0.2, 0.5, 0));
      Eigen::Matrix4f m = transform.matrix();
      std::vector<Eigen::Matrix4f> vm = {m};
      transformBuffer = renderer->createBuffer(vm.data(), vm.size() * sizeof(Eigen::Matrix4f), tz::BufferUsage::Uniform);
      transformDescriptor = new tz::Descriptor();
      transformDescriptor->buffer = transformBuffer;
      transformDescriptor->binding = &transformBinding;

      std::vector<Eigen::Vector3f> positions =
        {{-0.2, 0.2, 0},
        {-0.2, -0.2, 0},
        {0.2, -0.2, 0}};
      triangleVertexBuffer = renderer->createBuffer(positions.data(), positions.size() * sizeof(Eigen::Vector3f), tz::BufferUsage::Vertex);

    // The actual commands which shall be executed during the pipeline execution
    // are recorded into a commandbuffer.
    // One commandbuffer can hold many commands and also include multiple draw commands.
    // During one frame, more than one commandbuffer may be executed.
    // Currently, we only allow sequential execution though, multithreaded/parallel execution
    // is a future feature.
    commandBuffer = renderer->createCommandBuffer();

    

  }



  tz::ShaderPipeline* defaultShaderPipeline;
  tz::PipelineStateObject* defaultPSO;
  tz::CommandBuffer* commandBuffer = nullptr;
  tz::Buffer* transformBuffer = nullptr;
  tz::Buffer* triangleVertexBuffer = nullptr;
  tz::Descriptor* transformDescriptor = nullptr;

};

std::string getGLSLVertexShader() {
  return R"(
#version 460 core
layout(location = 0) in vec3 pos;

layout(std140, binding = 0) uniform Camera
{
  mat4 view;
  mat4 proj;
} camera;

layout(std140, binding = 1) uniform Transform
{
  mat4 world;
} transform;

void main() {
    //gl_Position = camera.proj * camera.view * transform.world * vec4(pos, 1);
    gl_Position = transform.world * vec4(pos, 1);
}
)";
}

std::string getGLSLFragmentShader()
{
  return R"(
    #version 460 core
    out vec4 color;
    void main() {
        color = vec4(1, 1, 1, 1);
    }

    )";
}


void runDemoGL()
{

  tz::Renderer* renderer = reinterpret_cast<tz::Renderer *>(new tz::OpenGLRenderer());
  auto ws = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = ws->createWindow(winDesc);
  renderer->init(window);

  const std::string& vertexShaderSource = getGLSLVertexShader();
  const std::string& fragmentShaderSource = getGLSLFragmentShader();
  auto vs = renderer->createShaderModule(tz::ShaderType::Vertex, vertexShaderSource);
  auto fs = renderer->createShaderModule(tz::ShaderType::Fragment, fragmentShaderSource);

  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto gameGraphicsData = GameGraphicsData(renderer, shaderPipeline);

  while (true)
  {
    ws->pollEvents();
    renderer->beginFrame();
    renderer->clearScreen();

    // Immediate style rendering, similar to how OpenGL 2.0
    // worked with its "glVertex" and "glNormal" calls.
    // In the background these immediate calls are interpreted and
    // mapped to "modern-gpu" architecture, uploading into VertexBuffers etc.
    // The framework does make its best to interpret the incoming immediate
    // commands efficiently, but immediate-mode will normally not be as performant
    // as custom built buffers, pipeline states and shaders.
    renderer->beginDraw(tz::PrimitiveType::Triangles);
    renderer->emitPosition({-0.1, 0.1, -0.1});
    renderer->emitPosition({-0.1, -0.1, -0.1});
    renderer->emitPosition({0.1, -0.1, -0.1});

    renderer->emitPosition({-0.09, 0.1, 0});
    renderer->emitPosition({0.11, -0.1, 0});
    renderer->emitPosition({0.11, 0.1, 0});
    renderer->endDraw();

    // Use modern command buffer approach.
    // Every drawing/pipeline execution is materialised through
    // specific commands which are recorded into a (reusable)
    // commandBuffer, which gets submitted.
    // Any number of command buffers may be submitted per frame.
    // Submission it not immediate execution, but collection of the buffer.
    // Update our transform buffer, making the object move:
    {
      static Eigen::Vector3f posOffset = {0, 0,0};
      posOffset += Eigen::Vector3f {0.000, -0.000, 0};
      auto transform = Eigen::Affine3f::Identity();
      transform.translate(posOffset);
      static float angle = 0.0f;
      angle += 0.0001f;
      transform.rotate(Eigen::AngleAxis(angle, Eigen::Vector3f::UnitZ()));
      gameGraphicsData.transformBuffer->updateData(transform.matrix().data(), sizeof(Eigen::Matrix4f));
    }
    gameGraphicsData.commandBuffer->begin();
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdBindPipeline (gameGraphicsData.defaultPSO ));
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdBindVertexBuffers ({gameGraphicsData.triangleVertexBuffer}));
    //gameGraphicsData.commandBuffer->recordCommand(new tz::CmdBindDescriptors({gameGraphicsData.transformDescriptor}, gameGraphicsData.defaultPSO));
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdDraw(3, 1, 0, 0));
    gameGraphicsData.commandBuffer->end();
    renderer->submitCommandBuffer(gameGraphicsData.commandBuffer);

    // Ending the frame is where all the drawing & computation work is actually being
    // done on the GPU.
    // For legacy frameworks as OpenGL this means all the actual draw calls are done.
    // For modern APIs such as Vulkan and DX12, it means the actual submission
    // of their internal command buffers, fence waiting etc.
    renderer->endFrame();


    // Final step for a frame to present the contents of the backbuffer
    // to the actual display.
    ws->present();
  }

}

// TODO move into math or other helper library (might even already exist!?)
Eigen::Matrix4f lookAt(const Eigen::Vector3f& eye,
                       const Eigen::Vector3f& center,
                       const Eigen::Vector3f& up)
{
  Eigen::Vector3f f = (center - eye).normalized();
  Eigen::Vector3f s = f.cross(up).normalized();
  Eigen::Vector3f u = s.cross(f);

  Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();

  // Rotation part (Indices are mat(row, col))
  mat(0,0) =  s.x();
  mat(0,1) =  s.y(); // Changed from (1,0)
  mat(0,2) =  s.z(); // Changed from (2,0)

  mat(1,0) =  u.x();
  mat(1,1) =  u.y();
  mat(1,2) =  u.z();

  mat(2,0) = -f.x();
  mat(2,1) = -f.y();
  mat(2,2) = -f.z();

  // Translation part (Now in the 4th Column)
  mat(0,3) = -s.dot(eye);
  mat(1,3) = -u.dot(eye);
  mat(2,3) =  f.dot(eye);

  return mat;
}

Eigen::Matrix4f perspective(float fovY, float aspect, float zNear, float zFar)
{
  float tanHalfFovy = std::tan(fovY * 0.5f);
  Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

  m(0,0) = 1.0f / (aspect * tanHalfFovy);
  m(1,1) = 1.0f / (tanHalfFovy);

  // Vulkan Depth [0, 1]
  m(2,2) = zFar / (zNear - zFar);
  m(2,3) = (zNear * zFar) / (zNear - zFar); // Translation moved to Col 3

  // Perspective Divide
  m(3,2) = -1.0f; // Moved from (2,3) to (3,2) for Column-Major

  return m;
}


#ifdef USE_VULKAN
void runDemoVulkan()
{
  tz::Renderer* renderer = reinterpret_cast<tz::Renderer *>(new tz::render::vulkan::VulkanRenderer());
  auto ws = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = ws->createWindow(winDesc);
  renderer->init(window);

  auto spvPath = "shader_binaries/default_shader_stage3.slang.spv";

  auto vs = renderer->createShaderModule(tz::ShaderType::Vertex, spvPath);
  auto fs = renderer->createShaderModule(tz::ShaderType::Fragment, spvPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  std::vector<tz::VertexPosColor> vertices =
  {
    {{-0.2, 0.2, 0.5}, {1, 1, 0}},
    {{-0.2, -0.2, 0.5}, {0, 1, 1}},
    {{0.2, -0.2, 0.5}, {0, 1, 1}}
  };
  auto triVertexBuffer = renderer->createBuffer(vertices.data(),
                                                vertices.size() * sizeof(tz::VertexPosColor),
                                                tz::BufferUsage::Vertex);

  std::vector<tz::VertexPosTexCoords> verticesPosTexCoord =
    {
      {{0.2, 0.5, 0.5}, {0, 1}},
      {{0.2, -0.5, 0.5}, {0, 0}},
      {{0.4, -0.5, 0.5},  {1, 0}},
      {{0.4, 0.5, 0.5}, {1, 1}}
    };
  std::vector<uint32_t> indices =
    {
      0, 1, 2,
      0, 2, 3
    };
  auto quadVertexBuffer = renderer->createBuffer(verticesPosTexCoord.data(),
                                                 verticesPosTexCoord.size() * sizeof (tz::VertexPosColor),
                                                 tz::BufferUsage::Vertex);

  auto quadIndexBuffer = renderer->createBuffer(indices.data(),
                                                indices.size() * sizeof(uint32_t),
                                                tz::BufferUsage::Index);



  // Descriptor layout and binding for the transformation matrix:
  // First we create the model, view, projection matrices:
  auto transform = Eigen::Affine3f::Identity();
  transform.translate(Eigen::Vector3f(-0.08, -0.08, 0));
  Eigen::Matrix4f tm = transform.matrix();
  Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
  std::vector<Eigen::Matrix4f> vm = {m};
  tz::TransformUniformBufferObject transformUBO;
  transformUBO.model = tm;
  transformUBO.view = lookAt({0, 0, -1.17}, {0, 0, 0}, {0, 1,0});
  transformUBO.proj = perspective(1.04f, 640.0f / 480.0f, 0.1f, 100.0f);

  // Next we store the matrices data into a buffer and setup the descriptor bindings, layouts and sets:
//  auto transformBuffer = renderer->createMultiframeBuffer(&transformUBO, sizeof(transformUBO), tz::BufferUsage::Uniform);
//  auto transformDescBinding = renderer->createDescriptorBinding(0, tz::ResourceType::Ubo, tz::ShaderType::Vertex, 1, transformBuffer);
//  auto textureDescBinding = renderer->createDescriptorBinding(1, tz::ResourceType::Sampler, tz::ShaderType::Fragment, 1, nullptr, textureImageView, sampler);
//  auto descriptorSetLayout = renderer->createDescriptorSetLayout({transformDescBinding, textureDescBinding});
//  auto descriptorSet = renderer->createMultiframeDescriptorSet(descriptorSetLayout, transformBuffer);


  // TODO question: how to handle the descriptorSets?
  // One one hand they are totally part of the PSO, so we should probably store them with the PSO.
  // OTOH the buffer and the descriptorSets are needed at command-record time and at least the buffers must be updated
  // regularly. So we may create them along the PSO but also provide accessors to use them after the initial creation
  // for updates and binding during command recording.
  auto colorOnlyPso = createColorOnlyPSO(renderer);
  auto texturedPso  = createTexturedPSO(renderer);

  // We create the commandbuffer once and record into it every frame:
  auto commandBuffer = renderer->createCommandBuffer();

  while (true)
  {
    ws->pollEvents();
    renderer->beginFrame();
    renderer->beginCommandBuffer(commandBuffer);

    // Next we record actual draw commands per mesh.
    // We also can use more advanced features like instancing etc.
    // but for now we keep it conceptually simple.
    // There is general 2 different items we can draw:
    // 1. different meshes, then we need a differently bound vertex buffer
    // 2. same mesh, but at different locations: we reuse the same vertex buffer,
    // but either bind different "transform descriptors" (matrices...) for each draw call
    // or we have an instanced setup as mentioned above, where we can pass in many
    // transforms in one draw call.

    // TODO: actual transformation
    {
      static Eigen::Vector3f posOffset = {0, 0,0};
      posOffset += Eigen::Vector3f {0.000, -0.000, 0};
      auto transform = Eigen::Affine3f::Identity();
      transform.translate(posOffset);
      static float angle = 0.0f;
      angle += 0.0001f;
      transform.rotate(Eigen::AngleAxis(angle, Eigen::Vector3f::UnitZ()));
      //gameGraphicsData.transformBuffer->updateData(transform.matrix().data(), sizeof(Eigen::Matrix4f));
    }

    renderer->recordCommand(commandBuffer,new tz::CmdBindPipeline (colorOnlyPso) );
    renderer->recordCommand(commandBuffer, new tz::CmdSetViewPorts({{0, 0, 640, 480}}));
    renderer->recordCommand(commandBuffer, new tz::CmdSetScissors({{0, 0, 640, 480}}));

    // colored triangle:
    renderer->recordCommand(commandBuffer, new tz::CmdBindVertexBuffers ({triVertexBuffer}));
    renderer->recordCommand(commandBuffer, new tz::CmdBindDescriptors(colorOnlyPso->descriptorSets, colorOnlyPso));
    renderer->recordCommand(commandBuffer, new tz::CmdDraw(3, 1, 0, 0));

    // textured quad::
    renderer->recordCommand(commandBuffer,new tz::CmdBindPipeline (texturedPso) );
    renderer->recordCommand(commandBuffer, new tz::CmdBindVertexBuffers ({quadVertexBuffer}));
    renderer->recordCommand(commandBuffer, new tz::CmdBindIndexBuffer(quadIndexBuffer, 0));
    renderer->recordCommand(commandBuffer, new tz::CmdBindDescriptors(texturedPso->descriptorSets, texturedPso));
    renderer->recordCommand(commandBuffer, new tz::CmdDrawIndexed(indices.size(), 1, 0, 0, 0));

    renderer->endCommandBuffer(commandBuffer);
    renderer->submitCommandBuffer(commandBuffer);


    renderer->endFrame();


    // Final step for a frame to present the contents of the backbuffer
    // to the actual display.
    ws->present();
  }
}
#endif


int main(int argc, char* argv[])
{
#ifndef USE_VULKAN
  runDemoGL();
#else
  runDemoVulkan();
#endif


  return 0;
}