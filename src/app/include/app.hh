#include <defines.h>
#include <functional>

#include <Eigen/Dense>
#include <window_system.hh>
#include <vulkan_renderer.hh>

namespace tz {
namespace rv =  render::vulkan;

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

  Eigen::Matrix4f getViewMatrix()
  {
    Eigen::Vector3f up = {0, 1, 0};
    Eigen::Vector3f f = (lookAt - pos).normalized();
    Eigen::Vector3f s = f.cross(up).normalized();
    Eigen::Vector3f u = s.cross(f);

    Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();

    // Set Columns (Eigen is Column-Major)
    mat.col(0).head<3>() = s;
    mat.col(1).head<3>() = u;
    mat.col(2).head<3>() = -f;

    // Translation part
    mat(0,3) = -s.dot(pos);
    mat(1,3) = -u.dot(pos);
    mat(2,3) =  f.dot(pos);

    return mat;
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
      m(1,1) = -1.0f / (tanHalfFovy);
      m(2,2) = zFar / (zNear - zFar);
      m(2,3) = (zNear * zFar) / (zNear - zFar);
      m(3,2) = -1.0f; // This must be at (3,2) for Eigen's Col-Major layout

      return m;
    }
    else if (type == CameraType::Ortho)
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


struct CameraUniformBufferObject
{
  Eigen::Matrix4f view;
  Eigen::Matrix4f proj;
};

struct alignas(16) TransformUniformBufferObject
{
  Eigen::Matrix4f model;

};

struct alignas(16) PerObjectUniformBufferObject
{
  Eigen::Matrix4f model;
  uint32_t textureId;
  uint32_t padding[3];

};

struct Transform
{
  Eigen::Vector3f position = {0, 0, 0};
  Eigen::Vector3f scale = {1, 1, 1};
  Eigen::Quaternionf orientation;
};

enum class VertexShaderType : int
{
  Static,
  Skeletal
};

enum class MaterialType
{
  SingleColor,
  DiffuseNormal,
  PBR,
};



/**
 * Intended use is for selecting PSOs efficiently.
 * Can be used to form a hashkey to select into an
 * unordered map of PSOs.
 *
 */
struct RenderHints
{
  MaterialType materialType = MaterialType::SingleColor;
  VertexShaderType vertexShaderType = VertexShaderType::Static;
  bool wireframe = false;
  bool depthTest = true;
  bool blending = true;
  rv::CullMode cullMode = rv::CullMode::Back;
  uint32_t texture;

  uint64_t getHash() const
  {
    uint64_t key = 0;
    key |= (static_cast<int>(materialType) & 0xFF);
    key |= (static_cast<int>(vertexShaderType) & 0xFF) << 8;
    key |= (wireframe? 1 : 0) << 16;
    key |= (depthTest? 1 : 0) << 17;
    key |= (blending? 1: 0)  << 18;
    key |= (static_cast<int>(cullMode) & 0xFF) << 19;

    return key;


  }
};


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





  class App;
  using FrameListener = std::function< void(App* app)>;

  class TZ_API App
  {


    public:
      App();
      virtual void run();
      virtual void setUpdateFunction(FrameListener frameListener);
      virtual float getLastFrameTime();

      virtual void activate3DCamera();
      virtual void activate3DCamera(Eigen::Vector3f position, Eigen::Vector3f lookAt);
      virtual void activateUICamera();
      virtual void activateUICamera(Eigen::Vector3f position);

      virtual void renderQuad(Transform transform, RenderHints renderHints = {});
      virtual void renderCube(Transform transform, RenderHints renderHints = {});
      virtual void renderSphere(Transform transform, RenderHints renderHints = {});
      virtual void renderCylinder(Transform transform, RenderHints renderHints = {});

      uint32_t createTexture(const std::string& imagePath);

  private:
      WindowSystem* windowSystem = nullptr;
      rv::Renderer* renderer = nullptr;

      std::vector<FrameListener> frameListeners;

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
      rv::PipelineStateObject* colorOnlyPSO = nullptr;
      rv::CommandBuffer* commandBuffer = nullptr;
      std::vector<PrimitiveRenderData> framePrimitives;

      rv::DescriptorSet* cameraDescriptorSet = nullptr;
      rv::DescriptorSet* perObjectDescriptorSet = nullptr;
      rv::DescriptorSet* diffuseTextureDescriptorSet = nullptr;
      rv::PipelineLayout* masterPipelineLayout = nullptr;

      uint32_t globalTextureIndex = 0;

      Camera* default3DCamera = nullptr;
      Camera* defaultUICamera = nullptr;
      Camera* activeRenderCamera = nullptr;

      void updateFrameListeners(float frameTime);
      void prepareRenderPrimitives();
      rv::PipelineStateObject* createColorOnlyPSO();
      Eigen::Matrix4f createPerspectiveProjectionMatrix(float fovY,
                                                        float aspect,
                                                        float zNear,
                                                        float zFar);
      Eigen::Matrix4f createLookAtMatrix(const Eigen::Vector3f &eye,
                                         const Eigen::Vector3f &center,
                                         const Eigen::Vector3f &up);

      std::unordered_map<uint64_t, rv::PipelineStateObject*> psoCache;
      void buildPSOCache();
      rv::PipelineStateObject *createTexturedPSO();
      void renderFrame();
      std::vector<PrimitiveRenderData> getRenderPrimitivesByCamera(Camera *camera);
      render::vulkan::Renderer *vulkanRenderer();
      void createMasterPipelineLayout();
      void renderPrimitives(const std::vector<PrimitiveRenderData> &primitives,
                            uint32_t &primitiveCounter);
  };


}

