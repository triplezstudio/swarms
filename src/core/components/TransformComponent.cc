
#include "TransformComponent.hh"
#include "AxisAlignedBoundingBox.hh"
#include "CircleBox.hh"
#include <cxxabi.h>

namespace swarms::core {

TransformComponent::TransformComponent(IBoundingBoxShPtr bbox)
  : AbstractComponent(ComponentType::TRANSFORM)
  , m_bbox(std::move(bbox))
{
  if (m_bbox == nullptr)
  {
    throw std::invalid_argument("Expected non null bounding box");
  }
}

auto TransformComponent::position() const -> Eigen::Vector3f
{
  return m_bbox->position();
}

auto TransformComponent::size() const -> float
{
  if (const auto circleBox = std::dynamic_pointer_cast<CircleBox>(m_bbox); circleBox != nullptr)
  {
    return circleBox->radius();
  }
  if (const auto aabb = std::dynamic_pointer_cast<AxisAlignedBoundingBox>(m_bbox); aabb != nullptr)
  {
    return aabb->dims().maxCoeff();
  }

  /// https://stackoverflow.com/questions/3649278/how-can-i-get-the-class-name-from-a-c-object
  int status;
  std::string name = ::abi::__cxa_demangle(typeid(*m_bbox).name(), nullptr, nullptr, &status);

  throw std::invalid_argument("Unsupported bounding box type " + name);
}

bool TransformComponent::contains(const Eigen::Vector3f &pos) const
{
  return m_bbox && m_bbox->isInside(pos);
}

void TransformComponent::translate(const Eigen::Vector3f &delta)
{
  m_bbox->translate(delta);
}

void TransformComponent::overridePosition(const Eigen::Vector3f &position)
{
  m_bbox->moveTo(position);
}

void TransformComponent::simulate(const time::TickData & /*data*/)
{
  // Voluntarily empty.
}

namespace {
const Eigen::Vector3f Z_AXIS = Eigen::Vector3f(0.0, 0.0, 1.0);
}

auto TransformComponent::transformToGlobal(const Eigen::Vector3f &localPos) const -> Eigen::Vector3f
{
  return m_bbox->position() + localPos;
}

} // namespace swarms::core
