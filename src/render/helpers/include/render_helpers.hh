//
// Created by Martin Gruscher on 19.05.26.
//

#ifndef SWARMS_RENDER_HELPERS_HH
#define SWARMS_RENDER_HELPERS_HH
#include <Eigen/Dense>
#include <vulkan_renderer.hh>
#include <text_helper.hh>

namespace tz
{

enum class PrimitiveGeometryType
{
  Line,
  Quad,
  Cube,
  Sphere,
  Mesh
};

enum class PrimitiveMaterialType
{
  SingleColor,
  DiffuseTexture,
  PBR,

};



struct Transform
{
  Eigen::Vector3f position = {0, 0, 0};
  Eigen::Vector3f scale = {1, 1, 1};
  Eigen::Quaternionf orientation;
};

namespace rv = render::vulkan;
/**
 * Intended use is for selecting PSOs efficiently.
 * Can be used to form a hashkey to select into an
 * unordered map of PSOs.
 *
 */
struct RenderHints
{

  rv::MaterialType materialType = rv::MaterialType::SingleColor;
  rv::VertexShaderType vertexShaderType = rv::VertexShaderType::Static;
  bool wireframe = false;
  bool depthTest = true;
  bool blending = true;
  rv::CullMode cullMode = rv::CullMode::Back;
  uint32_t texture;
  Eigen::Vector4f color = {1, 1, 1,1};

  rv::PSOCacheKey toCacheKey() const
  {
    rv::PSOCacheKey ck;
    ck.materialType = materialType;
    ck.vertexShaderType = vertexShaderType;
    ck.wireframe = wireframe;
    ck.depthTest = depthTest;
    ck.blending = blending;
    ck.cullMode = cullMode;

    return ck;
  }

};


inline void fillWithQuadVertices(std::vector<rv::VertexPos>& out)
{
  out = {{{-0.5, 0.5, 0.5}},
         {{-0.5, -0.5, 0.5}},
         {{0.5, -0.5, 0.5}},
         {{0.5, 0.5, 0.5}}};
}


inline void fillWithQuadVertices(std::vector<rv::VertexPosTexCoords>& out)
{

  out =
    {
      {{-0.5, 0.5, 0.5}, {0, 1}},
      {{-0.5, -0.5, 0.5}, {0, 0}},
      {{0.5, -0.5, 0.5},  {1, 0}},
      {{0.5, 0.5, 0.5}, {1, 1}}
    };

}

inline void fillWithCubeVertices(std::vector<rv::VertexPosTexCoords>& out)
{
  out = {
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
}

inline void fillWithCubeVertices(std::vector<rv::VertexPos>& out)
{
   out = {
    {{-.5f, -.5f,  .50f}}, // 0: Front-Bottom-Left
    {{ .5f, -.5f,  .5f}}, // 1: Front-Bottom-Right
    {{ .5f,  .5f,  .5f}}, // 2: Front-Top-Right
    {{-.5f,  .5f,  .5f}}, // 3: Front-Top-Left
    {{-.5f, -.5f, -.5f}}, // 4: Back-Bottom-Left
    {{ .5f, -.5f, -.5f}}, // 5: Back-Bottom-Right
    {{ .5f,  .5f, -.5f}}, // 6: Back-Top-Right
    {{-.5f,  .5f, -.5f}}  // 7: Back-Top-Left
  };
}

std::array<uint32_t, 6> TZ_API getQuadIndices();
std::array<uint32_t, 36> TZ_API getCubeIndices();
std::array<uint32_t, 36> TZ_API getCubeIndicesPosTex();

enum class CameraType
{
  Ortho,
  Perspective
};

class Camera
{
  public:
  [[maybe_unused]] Camera(Eigen::Vector3f pos, Eigen::Vector3f lookAt, CameraType type) : pos(pos), lookAt(lookAt), type(type)
  {

  }

  // Right-handed look-at view matrix for Vulkan // Vulkan uses: +X right, +Y down (in NDC), +Z forward (into screen) // World space convention here: +X right, +Y up, -Z forward (RH, like GLM default)
  Eigen::Matrix4f lookAtRH()
  {
    Eigen::Vector3f up = {0, 1, 0};
    // Forward vector (from eye to target), negated for RH
    Eigen::Vector3f f = (lookAt - pos).normalized(); // Right vector
    Eigen::Vector3f s = f.cross(up).normalized(); // Recomputed up (orthogonal)
    Eigen::Vector3f u = s.cross(f);
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    view(0, 0) = s.x();
    view(0, 1) = s.y();
    view(0, 2) = s.z();
    view(1, 0) = u.x();
    view(1, 1) = u.y();
    view(1, 2) = u.z();
    view(2, 0) = -f.x();
    view(2, 1) = -f.y();
    view(2, 2) = -f.z();
    view(0, 3) = -s.dot(pos);
    view(1, 3) = -u.dot(pos);
    view(2, 3) = f.dot(pos);
    return view;
  }

  Eigen::Matrix4f getProjectionMatrix(float width, float height)
  {
    if (type == CameraType::Perspective)
    {
      float fovY = 0.5236; // around 30 degress vertical fov
      float aspect = width / height;
      float tanHalfFovy = std::tan(fovY * 0.5f);
      Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

      // TODO customizable:

      float zFar = 1000;
      float zNear = 0.1;

      m(0,0) = 1.0f / (aspect * tanHalfFovy);
      m(1,1) = 1.0f / (tanHalfFovy);
      m(2,2) = zFar / (zNear - zFar);
      m(2,3) = (zNear * zFar) / (zNear - zFar);
      m(3,2) = -1.0f;

      return m;
    }
    else
    {
      float left = 0;
      float right = width;
      float top = height;
      float bottom = 0;

      // TODO customizable:
      float zFar = 100;
      float zNear = 0.1;

      Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

      m(0,0) = 2.0f / (right - left);
      m(1,1) = 2.0f / (top - bottom);
      m(2,2) = 1.0f / (zNear - zFar);   // Vulkan: [0,1] depth
      m(3,3) = 1.0f;

      m(0,3) = -(right + left) / (right - left);
      m(1,3) = -(top + bottom) / (top - bottom);
      m(2,3) = zNear / (zNear - zFar);

      return m;
    }
  }


  public:
  Eigen::Vector3f pos;
  Eigen::Vector3f lookAt;
  CameraType type;

};


class MasterPipelineLayout
{
  public:
  MasterPipelineLayout(rv::Renderer& renderer)
  {
    // Camera is set0, binding0
    auto cameraBuffer = renderer.createMultiframeUniformBuffer(2, sizeof(rv::CameraUniformBufferObject));
    auto cameraUBOBinding = renderer.createDescriptorBinding(0,
                                                             rv::DescriptorResourceType::Ubo,
                                                             rv::ShaderType::Vertex, 1,
                                                             cameraBuffer);
    auto cameraDescriptorSetLayout =  (renderer.createDescriptorSetLayout({cameraUBOBinding}));
    cameraDescriptorSet = renderer.createMultiframeDescriptorSet(cameraDescriptorSetLayout);

    // PerObject is set1, binding0
    auto perObjectBuffer = renderer.createMultiframeUniformBuffer(10000, sizeof(rv::PerObjectUniformBufferObject));
    auto perObjectUBOBinding = renderer.createDescriptorBinding(0, rv::DescriptorResourceType::Ubo,
                                                                rv::ShaderType::Vertex, 1,
                                                                perObjectBuffer);
    auto perObjectDescriptorSetLayout = renderer.createDescriptorSetLayout({perObjectUBOBinding});
    perObjectDescriptorSet = renderer.createMultiframeDescriptorSet(perObjectDescriptorSetLayout);

    // Diffuse textures at set2, binding0.
    // We allow up to 1000 textures
    auto textureDescBinding = renderer.createDescriptorBinding(0, rv::DescriptorResourceType::Sampler,
                                                               rv::ShaderType::Fragment, 1000, nullptr, nullptr);

    auto diffuseTextureDescriptorSetLayout = renderer.createDescriptorSetLayout({textureDescBinding}, true);
    diffuseTextureDescriptorSet = renderer.createMultiframeDescriptorSet(diffuseTextureDescriptorSetLayout);

    masterPipelineLayout = renderer.createPipelineLayout({cameraDescriptorSetLayout, perObjectDescriptorSetLayout, diffuseTextureDescriptorSetLayout});

  }

  rv::DescriptorSet* getPerObjectDescriptorSetPtr()
  {
    return perObjectDescriptorSet;
  }

  rv::DescriptorSet& getDiffuseTextureDescriptorSet() {
    return *diffuseTextureDescriptorSet;
  }

  rv::DescriptorSet* getDiffuseTextureDescriptorSetPtr() {
    return diffuseTextureDescriptorSet;
  }

  rv::PipelineLayout& getPipelineLayout() {
    return *masterPipelineLayout;
  }

  rv::PipelineLayout* getPipelineLayoutPtr()
  {
    return masterPipelineLayout;
  }

  rv::DescriptorSet& getCameraDescriptorSet()
  {
    return *cameraDescriptorSet;
  }

  rv::DescriptorSet* getCameraDescriptorSetPtr()
  {
    return cameraDescriptorSet;
  }

  private:
  rv::DescriptorSet* cameraDescriptorSet = nullptr;
  rv::DescriptorSet* diffuseTextureDescriptorSet = nullptr;
  rv::DescriptorSet* perObjectDescriptorSet = nullptr;
  rv::PipelineLayout* masterPipelineLayout = nullptr;


};


struct PrimitiveRenderData
{
  PrimitiveGeometryType geometryType;
  RenderHints renderHints;
  Transform transform;
  Camera* associatedCamera = nullptr;
  rv::Buffer* vertexBuffer = nullptr;
  rv::Buffer* indexBuffer = nullptr;
  uint32_t indexCount = 0;
};


}



#endif //SWARMS_RENDER_HELPERS_HH
