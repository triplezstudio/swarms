//
// Created by mgrus on 28.03.2026.
//

export module app:base;

import common;
import render.base;

namespace app_base {

  class AppImpl {
  public:
  virtual void init() = 0;
  virtual void doFrame() = 0;
  virtual render::Renderer* getRenderer() = 0;



  };
}



