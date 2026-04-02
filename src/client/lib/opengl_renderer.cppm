module;
#include "defines.h"
#include <Eigen/Dense>
#include <GL/glew.h>
#include <functional>
export module render.opengl;

import app;
import windowing;
import render.base;

import common;

export namespace tz {


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
class VertexBuffer
{
  public:
  /**
       * Create an initially empty vertex buffer.
       * This can be useful to implement dynamic streaming
       * and updating the buffer on the fly later.
       */
  VertexBuffer(uint64_t size)
    : sizeInBytes(size)
  {
    glCreateBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glNamedBufferStorage(handle,
                         size,
                         nullptr,
                         GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

  }

  template<typename T>
  VertexBuffer(std::vector<T> data)
  {
    glCreateBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    sizeInBytes = data.size() * sizeof(T);
    glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data.data(), GL_STATIC_DRAW);
  }

  template<typename T>
  void appendData(std::vector<T> data)
  {
    memcpy((uint8_t *) mappedPtr + currentAppendOffset, data.data(),
           data.size() * sizeof(T));
    currentAppendOffset += data.size() * sizeof(T);
    elementCounter += data.size();
  }

  uint64_t getNumberOfElements()
  {
    return elementCounter;
  }

  void bind()
  {
    glBindBuffer(GL_ARRAY_BUFFER, handle);
  }

  void unbind()
  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void clear()
  {
    currentAppendOffset = 0;
    elementCounter = 0;
  }

  void map()
  {
    mappedPtr = glMapNamedBufferRange(handle,
                                      0,
                                      sizeInBytes,
                                      GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
                                        | GL_MAP_COHERENT_BIT);
  }

  void unmap()
  {
    glUnmapNamedBuffer(handle);
  }

  GLuint getHandle()
  {
    return handle;
  }

  private:
  GLuint handle;
  uint64_t sizeInBytes;
  void *mappedPtr = nullptr;
  uint64_t currentAppendOffset = 0;
  uint32_t elementCounter = 0;


};

/**
 * Abstraction over an OpenGL VAO.
 * With builder pattern, makes it easier to construct.
 */
class VertexArrayObject
{
  public:
  GLuint getHandle()
  {
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

class OpenGLShaderPipeline : public tz::ShaderPipeline
{
  public:
      void link(std::vector<ShaderModule*> modules) override;

      void bind()
      {
        glUseProgram(programHandle);
      }

      void unbind()
      {
        glUseProgram(0);
      }

      void* getHandle() override
      {
        return &programHandle;
      }

  private:
      GLuint programHandle;
};

class OpenGLShaderModule : public tz::ShaderModule
{

  public:
      void init(ShaderType type, const std::string& source) override;
      void* getHandle() override
      {
        return &handle;
      }

  private:
      GLuint handle = 0;

};

/**
 * OpenGLRenderer is a custom renderer using the OpenGL 4.6 API.
 *
 */
class SWARMS_API OpenGLRenderer : public Renderer
{
  public:
  void init(tz::Window* window) override;
  WindowDesc getRequiredWindowDesc() override;

  void beginFrame();
  void endFrame();

  void clearScreen() override;


  // This implements the "immediate" command style API:
  void beginDraw(PrimitiveType primitiveType) override;
  void endDraw() override;
  void emitPosition(Eigen::Vector3f pos) override;
  void emitColor(Eigen::Vector4f color) override;
  void emitUV(Eigen::Vector2f uv) override;
  void emitNormal(Eigen::Vector3f normal) override;

  private:
  GLuint createVertexArrayObject();
  GLuint createVertexBuffer(VertexBufferCreateInfo vbCreateInfo);

  private:
  tz::Window* window = nullptr;

  VertexBuffer *immediatePerFrameBuffer = nullptr;
  GLuint immediatePerFrameVAO = 0;

  // These are used to store the current immediate-stream-data positions.
  // So these are normally short-lived.
  // Normally when endDraw is called, these are copied into the
  // immediatePerFrameBuffer.
  std::vector<Eigen::Vector3f> positions;

  // This is the shader pipeline which is used for
  // all non-specific renderings, e.g for the interpretation of the
  // immediate-style commands and the primitive-rendering commands (drawCube, drawSphere..)
  OpenGLShaderPipeline* defaultShaderPipeline = nullptr;
};

} // namespace render