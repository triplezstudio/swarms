
#pragma once

#include "IBoundingBox.hh"

namespace swarms::core {

class AxisAlignedBoundingBox : public IBoundingBox
{
  public:
  AxisAlignedBoundingBox(const Eigen::Vector3d &center, const Eigen::Vector3d &dims);
  ~AxisAlignedBoundingBox() override = default;

  auto dims() const -> Eigen::Vector3d;

  auto position() const -> Eigen::Vector3d override;
  void moveTo(const Eigen::Vector3d &position) override;
  bool isInside(const Eigen::Vector3d &pos) const override;

  void translate(const Eigen::Vector3d &delta) override;

  private:
  Eigen::Vector3d m_center{Eigen::Vector3d::Zero()};
  Eigen::Vector3d m_dims{Eigen::Vector3d::Ones()};
};

} // namespace swarms::core
