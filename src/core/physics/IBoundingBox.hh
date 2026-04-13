
#pragma once

#include <eigen3/Eigen/Eigen>
#include <memory>

namespace swarms::core {

class IBoundingBox
{
  public:
  virtual ~IBoundingBox() = default;

  virtual auto position() const -> Eigen::Vector3d        = 0;
  virtual void moveTo(const Eigen::Vector3d &position)    = 0;
  virtual bool isInside(const Eigen::Vector3d &pos) const = 0;

  virtual void translate(const Eigen::Vector3d &delta) = 0;
};

using IBoundingBoxPtr   = std::unique_ptr<IBoundingBox>;
using IBoundingBoxShPtr = std::shared_ptr<IBoundingBox>;

} // namespace swarms::core
