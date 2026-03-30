
#pragma once

#include "IEnvironment.hh"

namespace swarms::core {

class AbstractEnvironment : public IEnvironment
{
  public:
  ~AbstractEnvironment() override = default;

  void simulate(const time::TickData &data) override;

  protected:
  AbstractEnvironment() = default;

  /// @brief - Performs necessary operations to prepare the data needed
  /// by agents to execute their behavior.
  /// This method is guaranteed to be called once for each invocation of
  /// the `simulate` method.
  /// @param data - information about the time elapsed since the last
  /// call to this method.
  virtual void computePreAgentsStep(const time::TickData &data) = 0;

  /// @brief - Executes the behavior of the agents, using the perceptions
  /// available to them and outputs the result. No action is applied in
  /// this step: they will all be resolved in the post agents step.
  /// This method is guaranteed to be called once for each invocation of
  /// the `simulate` method.
  /// @param data - information about the time elapsed since the last
  /// call to this method.
  virtual void computeAgentsStep(const time::TickData &data) = 0;

  /// @brief - Executes the necessary operations to apply the result of
  /// the behaviors of the agents to the world. This step also takes care
  /// of solving conflicts that might arise when multiple agents want to
  /// modify the world in the same way.
  /// This method is guaranteed to be called once for each invocation of
  /// the `simulate` method.
  /// @param data - information about the time elapsed since the last
  /// call to this method.
  virtual void computePostAgentsStep(const time::TickData &data) = 0;
};

} // namespace swarms::core
