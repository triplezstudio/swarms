#pragma once
#include "defines.h"
#include <Eigen/Dense>
#include <GL/glew.h>
#include <functional>
#include <iostream>
#include <map>
#include "render.cppm"

namespace tz {

struct VertexBufferCreateInfo
{
  std::vector<float> data;

};

struct VertexBufferUpdateInfo
{
  std::vector<float> data;
};

class OpenGLBuffer : public Buffer
{
  public:
      OpenGLBuffer(uint64_t sizeInBytes, void* data)
      {

        glCreateBuffers(1, &handle);
        glNamedBufferStorage(handle, sizeInBytes, data, GL_DYNAMIC_STORAGE_BIT);
      }

      void updateData(uint64_t sizeInBytes, void* data) override
      {
        glNamedBufferSubData(
          handle,
          0,
          sizeInBytes,
          data
          );
      }

      void* getHandle()
      {
        return &handle;
      }

  private:
      GLuint handle;
};

/**
 * Abstraction over an OpenGL VBO.
 */
class OpenGLVertexBuffer : public VertexBuffer
{
  public:
  /**
  * Create an initially empty vertex buffer.
  * This can be useful to implement dynamic streaming
  * and updating the buffer on the fly later.
  */
  OpenGLVertexBuffer(uint64_t size): sizeInBytes(size)
  {
    glCreateBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glNamedBufferStorage(handle,
                         size,
                         nullptr,
                         GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

  }

  template<typename T>
  OpenGLVertexBuffer(std::vector<T> data)
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

  void* getHandle()
  {
    return &handle;
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
class OpenGLVertexArrayObject
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
      vertexArrayObject = new OpenGLVertexArrayObject();
    }

    Builder *positions(std::vector<Eigen::Vector3f> pos)
    {
      this->_positions = pos;
      return this;
    }

    // TODO add further attributes, e.g. normals, uvs, etc.

    OpenGLVertexArrayObject *build()
    {
      glGenVertexArrays(1, &vertexArrayObject->handle);
      glBindVertexArray(vertexArrayObject->handle);

      auto vbo = OpenGLVertexBuffer(_positions);

      return vertexArrayObject;
    }

private:
    OpenGLVertexArrayObject *vertexArrayObject = nullptr;
    std::vector<Eigen::Vector3f> _positions;
  };
};

class SWARMS_API OpenGLShaderPipeline : public ShaderPipeline
{
  public:
      void link(const std::vector<ShaderModule*>& modules) override;

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

class SWARMS_API OpenGLShaderModule : public ShaderModule
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

  void beginFrame() override;
  void endFrame() override;

  void clearScreen() override;


  // This implements the "immediate" command style API:
  void beginDraw(PrimitiveType primitiveType) override;
  void endDraw() override;
  void emitPosition(Eigen::Vector3f pos) override;
  void emitColor(Eigen::Vector4f color) override;
  void emitUV(Eigen::Vector2f uv) override;
  void emitNormal(Eigen::Vector3f normal) override;

  void submitCommandBuffer(tz::CommandBuffer *commandBuffer) override;

  VertexBuffer* createVertexBuffer(const std::vector<Eigen::Vector3f>& data) override;

  Buffer * createBuffer(uint64_t sizeInBytes, void *data) override;

  private:
  GLuint createVertexArrayObject();
  GLuint createVertexBuffer(VertexBufferCreateInfo vbCreateInfo);
  void executeImmediateCommands();
  void executeCommandBuffers();
  void executeCommandBuffer(CommandBuffer* commandBuffer);

  void execCmdBindPipeline(CmdBindPipeline *cmd);
  void execCmdBindVertexBuffers(CmdBindVertexBuffers* cmd);
  void execCmdDraw(CmdDraw* cmd);
  void execCmdBindDescriptors(CmdBindDescriptors* cmd);

  GLenum primitiveTypeToEnum(PrimitiveType pt);
  GLenum dataTypeToEnum(DataType dt);
  GLenum resourceTypeToEnum(ResourceType rt);

  private:
  tz::Window* window = nullptr;

  OpenGLVertexBuffer *immediatePerFrameBuffer = nullptr;
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

  PipelineStateObject* currentPSO = nullptr;

  // By submitting commandbuffers, they are collected here.
  std::vector<CommandBuffer*> frameCommandBuffers;

  std::map<std::string, GLuint> vaoCache;
};

} // namespace render