#pragma once
#include <string>
#include <functional>

namespace tz {

// Helper struct to hold whatever the platform needs
struct NativeHandles
{
  void *connection;
  void *window;
  std::string videoSubSystem;
};


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

};


struct Window
{
  void* nativeHandle = nullptr;
  int width;
  int height;
  std::function<GraphicsSurface (GraphicsInstance&)> surfaceCreationFunc;
  std::function<void(int* width, int* height)> displaySizeFunc;
};




}