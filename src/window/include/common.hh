#pragma once
#include <string>

namespace client_common {

// Helper struct to hold whatever the platform needs
struct NativeHandles
{
  void *connection;
  void *window;
  std::string videoSubSystem;
};




}