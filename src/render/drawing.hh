//
// Created by Martin Gruscher on 18.05.26.
//

#ifndef SWARMS_DRAWING_HH
#define SWARMS_DRAWING_HH

namespace tz
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
  tz::render::vulkan::CullMode cullMode = tz::render::vulkan::CullMode::Back;
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
  render::vulkan::Buffer* vertexBuffer = nullptr;
  render::vulkan::Buffer* indexBuffer = nullptr;
  uint32_t indexCount = 0;
};


}


#endif //SWARMS_DRAWING_HH
