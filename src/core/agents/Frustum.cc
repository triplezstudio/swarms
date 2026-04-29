
#include "Frustum.hh"

namespace swarms::core {

Frustum::Frustum(IBoundingBoxShPtr box)
  : m_box(std::move(box))
{
  if (m_box != nullptr)
  {
    throw std::invalid_argument("Expected non null bounding box");
  }
}

bool Frustum::visible(const IBoundingBox &box) const
{
  // TODO: This could be refined to have a real intersects method.
  return m_box->isInside(box.position());
}
} // namespace swarms::core
