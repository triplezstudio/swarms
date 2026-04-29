
#pragma once

#include "CoreObject.hh"
#include "IEnvironment.hh"
#include "ITimeManager.hh"
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace swarms::core {

class EnvironmentProcessor : public runtime::CoreObject
{
  public:
  EnvironmentProcessor(IEnvironmentShPtr environment, time::ITimeManagerPtr timeManager);
  ~EnvironmentProcessor() override;

  void start();
  void stop();

  private:
  std::mutex m_locker{};
  bool m_running{false};
  std::thread m_processingThread{};

  time::ITimeManagerPtr m_timeManager{};
  IEnvironmentShPtr m_environment{};

  void asyncProcessing();

  /// @brief - Used to check whether the asynchronous processing was started and
  /// if yes, stops it.
  /// This method aims at wrapping acquiring the `m_locker` in a dedicated block
  /// to make the join call possible on the processing thread.
  /// @return - true if the processing thread was started and needs to be joined
  bool tryStopProcessing();
};

using EnvironmentProcessorPtr = std::unique_ptr<EnvironmentProcessor>;

} // namespace swarms::core
