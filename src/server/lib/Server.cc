

#include "Server.hh"
#include "Environment.hh"
#include "RNG.hh"
#include "RandomInitializer.hh"
#include "TimeManager.hh"

namespace swarms::server {

Server::Server()
  : runtime::CoreObject("server")
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

namespace {
// The simulation always starts from the beginning. This could be changed
// in the future to load the last reached tick from a database or another
// persistence layer.
const time::Tick INITIAL_TICK{0.0};

// This represents the mapping between one real world second and a second
// in the simulation. This configuration means that one real world second
// is equivalent to one second in the simulation. Having larger or smaller
// time steps allow to speed up/slow down the simulation.
const time::TimeStep SIMULATION_TIME_STEP{1, time::Duration{time::Unit::SECONDS, 1.0}};
} // namespace

void Server::initialize()
{
  m_environment = std::make_shared<core::Environment>();
  m_processor   = std::make_unique<core::EnvironmentProcessor>(
    m_environment, std::make_unique<time::TimeManager>(INITIAL_TICK, SIMULATION_TIME_STEP));

  core::RNG rng;
  simulation::RandomInitializer initializer(simulation::InitializationConfig{});
  initializer.setup(*m_environment, rng);
}

void Server::setup()
{
  m_processor->start();
}

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

void Server::shutdown()
{
  m_processor->stop();
}

} // namespace swarms::server
