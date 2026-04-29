
#pragma once

#include "IBoundingBox.hh"

namespace swarms::core {

class Frustum
{
  public:
  Frustum(IBoundingBoxShPtr box);
  ~Frustum() = default;

  bool visible(const IBoundingBox &box) const;

  private:
  IBoundingBoxShPtr m_box{};
};

} // namespace swarms::core
