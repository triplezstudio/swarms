

#include "CircleBox.hh"

namespace swarms::core {

CircleBox::CircleBox(const Eigen::Vector3d &center, const double radius)
  : m_center(center)
  , m_radius(radius)
  , m_squaredRadius(m_radius * m_radius)
{}

auto CircleBox::radius() const -> double
{
  return m_radius;
}

auto CircleBox::position() const -> Eigen::Vector3d
{
  return m_center;
}

void CircleBox::moveTo(const Eigen::Vector3d &position)
{
  m_center = position;
}

bool CircleBox::isInside(const Eigen::Vector3d &pos) const
{
  return (pos - m_center).squaredNorm() < m_squaredRadius;
}

void CircleBox::translate(const Eigen::Vector3d &delta)
{
  m_center += delta;
}

} // namespace swarms::core
