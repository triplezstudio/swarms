
#pragma once

#include "AbstractComponent.hh"
#include <eigen3/Eigen/Eigen>
#include <optional>

namespace swarms::core {

enum class SpeedMode
{
  FIXED,
  VARIABLE
};

struct VelocityData
{
  double maxAcceleration{1.0};
  double maxSpeed{1.0};

  std::optional<Eigen::Vector3d> initialSpeed{};

  SpeedMode speedMode{SpeedMode::VARIABLE};
};

class VelocityComponent : public AbstractComponent
{
  public:
  VelocityComponent(const VelocityData &data);
  ~VelocityComponent() override = default;

  auto speedMode() const -> SpeedMode;
  auto acceleration() const -> Eigen::Vector3d;
  auto speed() const -> Eigen::Vector3d;
  auto maxSpeed() const -> double;

  /// @brief - Set the acceleration to its maximum allowed value in a direction
  /// equal to the input argument.
  /// @param direction - the direction to accelerate into.
  void accelerate(const Eigen::Vector3d &direction);

  /// @brief - Override the existing acceleration with the provided value without
  /// performing any checks.
  /// @param acceleration - the acceleration to set.
  void overrideAcceleration(const Eigen::Vector3d &acceleration);

  /// @brief - Set the speed to its maximum allowed value in a direction equal to
  /// the input argument.
  /// @param direction - the direction towards which the speed points to.
  void setSpeed(const Eigen::Vector3d &direction);

  /// @brief - Override the existing speed with the provided value without
  /// performing any checks.
  /// @param speed - the speed to set.
  void overrideSpeed(const Eigen::Vector3d &speed);

  void immobilize();

  private:
  SpeedMode m_speedMode{SpeedMode::VARIABLE};
  double m_maxAcceleration{1.0};
  double m_maxSpeed{1.0};

  Eigen::Vector3d m_acceleration{Eigen::Vector3d::Zero()};
  Eigen::Vector3d m_speed{Eigen::Vector3d::Zero()};
};

} // namespace swarms::core
