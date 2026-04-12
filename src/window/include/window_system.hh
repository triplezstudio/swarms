#pragma once
#include "common.hh"
#include <string>
#include <functional>

namespace tz {

struct GraphicsSurface {
  void* handle;
};

struct GraphicsInstance
{
  void* handle;
};


struct WindowDesc {
  int width;
  int height;
  std::string title;

  enum class GraphicsAPI {
    OpenGL,
    Vulkan,
    DX11,
    DX12
  } api;

  struct {
    int major;
    int minor;
    bool coreProfile;
  } gl;

  struct {
    bool enableValidation;
  } vk;


};

struct Window
{
  void* nativeHandle = nullptr;
  int width;
  int height;
  std::function<GraphicsSurface (GraphicsInstance&, WindowDesc windowDesc)> surfaceCreationFunc;
};


  class WindowSystem {
  public:
  virtual void init() = 0;
  virtual void pollEvents() = 0;
  virtual Window* createWindow(WindowDesc desc) = 0;
  virtual void present() = 0;
  virtual GraphicsSurface createSurface(GraphicsInstance& instance, WindowDesc desc) = 0;

  };
}



