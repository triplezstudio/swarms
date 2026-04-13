

#pragma once

#include "CoreObject.hh"
#include "EnvironmentProcessor.hh"
#include <atomic>
#include <condition_variable>

namespace swarms::server {
class Server : public runtime::CoreObject
{
  public:
  Server();
  ~Server() override = default;

  void run();
  void requestStop();

  private:
  std::atomic_bool m_running{false};
  std::mutex m_runningLocker{};
  std::condition_variable m_runningNotifier{};

  core::IEnvironmentShPtr m_environment{};
  core::EnvironmentProcessorPtr m_processor{};

  void initialize();

  void setup();
  void activeRunLoop();
  void shutdown();
};

} // namespace swarms::server
