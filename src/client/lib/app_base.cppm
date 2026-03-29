//
// Created by mgrus on 28.03.2026.
//

export module app:base;

import common;

namespace app_base {

  class AppImpl {
  public:
  virtual void init() = 0;
  virtual void run() = 0;
  };
}



