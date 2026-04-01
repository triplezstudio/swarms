module;
#include "defines.h"
#include <Eigen/Dense>
#include <GL/glew.h>
#include <functional>
export module render.opengl;

import render.base;

import common;

export namespace render {

struct OpenGLInitData
{
  client_common::NativeHandles nativeHandles;
  std::function<void(int *width, int *height)> displaySizeFunc;
};

struct ShaderHandle
{
  uint32_t id;
};

struct ProgramHandle
{
  uint32_t id;
};

struct TextureHandle
{
  uint32_t id;
};

struct VertexBufferHandle
{
  uint32_t id;
};

struct IndexBufferHandle
{
  uint32_t id;
};

struct VertexBufferCreateInfo
{
  std::vector<float> data;
  size_t element_size = 0;
  size_t stride       = 0;
};

struct VertexBufferUpdateInfo
{
  std::vector<float> data;
  size_t element_size = 0;
  size_t stride       = 0;
  VertexBufferHandle oldVBO;
};

/**
 * Abstraction over an OpenGL VBO.
 */
class VertexBuffer {

  public:
      VertexBuffer(std::vector<Eigen::Vector3f> data)
      {
        glCreateBuffers(1, &handle);
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        auto size = data.size() * sizeof(float) * 3;
        glBufferData(GL_ARRAY_BUFFER, size , data.data(), GL_STATIC_DRAW);
      }


      GLuint getHandle()
      {
        return handle;
      }

  private:
      GLuint handle;

};

/**
 * Abstraction over an OpenGL VAO.
 * With builder pattern, makes it easier to construct.
 */
class VertexArrayObject
{
  public:

  GLuint getHandle() {
    return handle;
  }

  private:
      GLuint handle = 0;

  public:
      class Builder
      {
    public:
        Builder()
        {
          vertexArrayObject = new VertexArrayObject();
        }

        Builder *positions(std::vector<Eigen::Vector3f> pos)
        {
          this->_positions = pos;
          return this;
        }

        // TODO add further attributes, e.g. normals, uvs, etc.

        VertexArrayObject *build()
        {

          glGenVertexArrays(1, &vertexArrayObject->handle);
          glBindVertexArray(vertexArrayObject->handle);

          auto vbo = VertexBuffer(_positions);

          return vertexArrayObject;
        }

    private:
        VertexArrayObject *vertexArrayObject = nullptr;
        std::vector<Eigen::Vector3f> _positions;
      };
};

/**
 * OpenGLRenderer is a custom renderer using the OpenGL 4.6 API.
 *
 */
class SWARMS_API OpenGLRenderer : public render::Renderer
{
  public:
  OpenGLRenderer(const OpenGLInitData &initData);
  void init() override;
  void beginDraw(render::PrimitiveType primitiveType) override;
  void endDraw() override;
  void emitPosition(Eigen::Vector3f pos) override;
  void emitColor(Eigen::Vector4f color) override;
  void emitUV(Eigen::Vector2f uv) override;
  void emitNormal(Eigen::Vector3f normal) override;

  private:
  GLuint createVertexArrayObject();
  GLuint createVertexBuffer(render::VertexBufferCreateInfo vbCreateInfo);

  private:
  OpenGLInitData initData;
};

} // namespace render