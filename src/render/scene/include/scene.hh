//
// Created by mgrus on 14.05.2026.
//

#ifndef SWARMS_SCENE_HH
#define SWARMS_SCENE_HH

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vulkan_renderer.hh>

namespace rv= tz::render::vulkan;
namespace tz::scene
{

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
  Text,
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
      m(1,1) = 1.0f / (tanHalfFovy);
      m(2,2) = zFar / (zNear - zFar);
      m(2,3) = (zNear * zFar) / (zNear - zFar);
      m(3,2) = -1.0f;

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




class Scene {

};
}

#endif //SWARMS_SCENE_HH
