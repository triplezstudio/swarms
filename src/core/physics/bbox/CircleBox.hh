
#pragma once

#include "IBoundingBox.hh"
#include <eigen3/Eigen/Eigen>

namespace swarms::core {

class CircleBox : public IBoundingBox
{
  public:
  CircleBox(const Eigen::Vector3d &center, const double radius);
  ~CircleBox() override = default;

  auto radius() const -> double;

  auto position() const -> Eigen::Vector3d override;
  void moveTo(const Eigen::Vector3d &position) override;
  bool isInside(const Eigen::Vector3d &pos) const override;

  void translate(const Eigen::Vector3d &delta) override;

  private:
  Eigen::Vector3d m_center{Eigen::Vector3d::Zero()};
  double m_radius{1.0};
  double m_squaredRadius{1.0};
};

} // namespace swarms::core
