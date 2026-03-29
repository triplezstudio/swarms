
module;
#include "defines.h"
#include <vulkan/vulkan_raii.hpp>
export module render.vulkan;

import render.base;

import common;

export namespace render::vulkan {

class SWARMS_API VulkanRenderer : public render::Renderer
{
  public:
  VulkanRenderer(std::vector<const char*> extensionNames);
  void init(client_common::NativeHandleProvider *handleProvider) override;
  void draw() override;

  private:
  void initSurface();

  private:
  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;
  vk::raii::SurfaceKHR *surface;
  client_common::NativeHandleProvider *handleProvider = nullptr;
  std::vector<const char*> extensions;
};
}
