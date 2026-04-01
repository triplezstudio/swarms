

import app;
import render.base;
import render.vulkan;

int main(int argc, char* argv[])
{
  app::App app;
  app.init();
  auto renderer = app.getRenderer();

  app.addFrameListener([renderer](float frameTime) {

        renderer->beginDraw(render::PrimitiveType::Triangles);
        renderer->emitPosition({-0.5, 0.5, 0});
        renderer->emitPosition({-0.5, -0.5, 0});
        renderer->emitPosition({0.5, -0.5, 0});
        renderer->endDraw();

     });
  app.run();

  return 0;
}