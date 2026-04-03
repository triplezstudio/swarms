module;
#include <string>
export module windowing;

import common;

export namespace tz {

struct Window
{
  void* nativeHandle = nullptr;
  int width;
  int height;

};

struct WindowDesc {
  int width;
  int height;
  std::string title;

  enum class GraphicsAPI {
    None,
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

  class WindowSystem {
  public:
  virtual void init() = 0;
  virtual void pollEvents() = 0;
  virtual Window* createWindow(WindowDesc desc) = 0;
  virtual void present() = 0;

  };
}



