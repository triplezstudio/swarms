

#pragma once

#include "CoreObject.hh"
#include <atomic>
#include <condition_variable>
#include <unordered_map>

namespace swarms {
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

  void initialize();

  void setup();
  void activeRunLoop();
  void shutdown();
};

} // namespace swarms
