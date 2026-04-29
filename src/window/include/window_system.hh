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
    Vulkan,
  } api;

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
  std::function<void(int* width, int* height)> displaySizeFunc;
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



