// Factor out the actual client implementation code so
// we keep the client "main" small and tidy.
// Makes it easier to swap alternative client (demo) applications.

#include "render.hh"
#include <window_helpers.hh>

void runAppWithStateApi()
{

  tz::initWindow(800, 600, "swarms client 0.0.1");
  while (true)
  {
    tz::beginDrawing();

    tz::endDrawing();
  }
}