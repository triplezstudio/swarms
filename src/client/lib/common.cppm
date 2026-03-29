module;
#include <string>
export module common;

export namespace client_common {

// Helper struct to hold whatever the platform needs
struct NativeHandles {
  void* connection;
  void* window;
  std::string videoSubSystem;
};

 class NativeHandleProvider {
  public:
  virtual NativeHandles getNativeHandles() = 0;
};
}


