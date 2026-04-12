
#include "AxisAlignedBoundingBox.hh"

namespace swarms::core {

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Eigen::Vector3d &center,
                                               const Eigen::Vector3d &dims)
  : m_center(center)
  , m_dims(dims)
{}

auto AxisAlignedBoundingBox::dims() const -> Eigen::Vector3d
{
  return m_dims;
}

auto AxisAlignedBoundingBox::position() const -> Eigen::Vector3d
{
  return m_center;
}

void AxisAlignedBoundingBox::moveTo(const Eigen::Vector3d &position)
{
  m_center = position;
}

bool AxisAlignedBoundingBox::isInside(const Eigen::Vector3d &pos) const
{
  Eigen::Vector3d frontBottomLeft = m_center - m_dims / 2.0;
  Eigen::Vector3d backTopRight    = m_center + m_dims / 2.0;

  if (pos(0) < frontBottomLeft(0))
  {
    return false;
  }
  if (pos(0) > backTopRight(0))
  {
    return false;
  }

  if (pos(1) < frontBottomLeft(1))
  {
    return false;
  }
  if (pos(1) > backTopRight(1))
  {
    return false;
  }

  if (pos(2) < frontBottomLeft(2))
  {
    return false;
  }
  if (pos(2) > backTopRight(2))
  {
    return false;
  }

  return true;
}

void AxisAlignedBoundingBox::translate(const Eigen::Vector3d &delta)
{
  m_center += delta;
}

} // namespace swarms::core
