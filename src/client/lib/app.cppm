module;
#include <defines.h>
export module app;

import :base;


export namespace app {

  class SWARMS_API App
  {
    public:
      App();
      virtual void init();
      virtual void run();
  private:
      app_base::AppImpl* impl = nullptr;

  };


}

