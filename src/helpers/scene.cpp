
#include <scene_helper.hh>

tz::Scene::Scene(Camera &camera) : camera(camera)
{

}
void tz::Scene::addNode(SceneNode &node)
{
  nodes.push_back(node);
}