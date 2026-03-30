
#pragma once

#include "AbstractComponent.hh"
#include "IBoundingBox.hh"

namespace swarms::core {

class TransformComponent : public AbstractComponent
{
  public:
  TransformComponent(IBoundingBoxShPtr bbox);
  ~TransformComponent() override = default;

  auto position() const -> Eigen::Vector3f;
  auto size() const -> float;
  bool contains(const Eigen::Vector3f &pos) const;

  void translate(const Eigen::Vector3f &delta);
  void overridePosition(const Eigen::Vector3f &position);

  void simulate(const time::TickData &data) override;

  /// @brief - Transforms the position provided in local coordinate space to a
  /// position in the global coordinate frame by applying the transform defined
  /// by this component to it.
  /// @param localPos - the position to transform, expressed in the local coordinate
  /// frame
  /// @return - the position expressed in the global coordinate frame
  auto transformToGlobal(const Eigen::Vector3f &localPos) const -> Eigen::Vector3f;

  private:
  IBoundingBoxShPtr m_bbox{};
};

using TransformComponentShPtr = std::shared_ptr<TransformComponent>;

} // namespace swarms::core
