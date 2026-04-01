

#include "Server.hh"

namespace swarms {

Server::Server()
{
  initialize();
}

void Server::run()
{
  debug("Starting server...");
  setup();

  activeRunLoop();

  debug("Shutting down server...");
  shutdown();
}

void Server::requestStop()
{
  m_running.store(false);
  std::unique_lock lock(m_runningLocker);
  m_runningNotifier.notify_one();
}

void Server::initialize() {}

void Server::setup() {}

void Server::activeRunLoop()
{
  m_running.store(true);
  bool running{true};

  while (running)
  {
    std::unique_lock lock(m_runningLocker);
    m_runningNotifier.wait(lock, [this] { return !m_running.load(); });

    running = m_running.load();
  }
}

void Server::shutdown() {}

} // namespace swarms
