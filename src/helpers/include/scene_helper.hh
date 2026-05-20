//
// Created by Martin Gruscher on 19.05.26.
//

#ifndef SWARMS_SCENE_HELPER_HH
#define SWARMS_SCENE_HELPER_HH
#include "render_helpers.hh"

namespace tz
{

/**
 *
 */
class SceneNode
{
  Transform transform;
  PrimitiveGeometryType geometryType;
};

/**
 * A scene consists of a camera,
 * lights and renderable objects.
 * The application may consist of several scenes,
 * e.g. one with a 2d camera for the ui,
 * one with a 3d camera for the main scene,
 * and maybe another one in 3d, which "films" the world from above
 * to produce a top-down mini-map.
 *
 * Each scene has a render target, either the main framebuffer,
 * so it would just show up directly on the display, or a
 * render-texture.
 * For the example above, the top-down-scene would render into a texture,
 * which the 2d ui scene would then use to draw the mini-map in a certain are of the screen.
 */
class TZ_API Scene
{
public:
  Scene(Camera& camera);
  void addNode(SceneNode& node);
  void removeNode(SceneNode& node);


private:
  Camera& camera;
  std::vector<SceneNode> nodes;

};


} // namespace tz

#endif //SWARMS_SCENE_HELPER_HH
