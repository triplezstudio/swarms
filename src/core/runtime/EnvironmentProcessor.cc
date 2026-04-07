
#include "EnvironmentProcessor.hh"
#include "TimeStamp.hh"

namespace swarms::core {

EnvironmentProcessor::EnvironmentProcessor(IEnvironmentShPtr environment,
                                           time::ITimeManagerPtr timeManager)
  : runtime::CoreObject("environment")
  , m_timeManager(std::move(timeManager))
  , m_environment(std::move(environment))
{
  addModule("processor");

  if (m_timeManager == nullptr)
  {
    throw std::invalid_argument("Expected non null time manager");
  }
  if (m_environment == nullptr)
  {
    throw std::invalid_argument("Expected non null environment");
  }
}

EnvironmentProcessor::~EnvironmentProcessor()
{
  stop();
}

void EnvironmentProcessor::start()
{
  const std::lock_guard guard(m_locker);
  if (m_running)
  {
    warn("Processor already started");
    return;
  }

  m_running          = true;
  m_processingThread = std::thread(&EnvironmentProcessor::asyncProcessing, this);
}

void EnvironmentProcessor::stop()
{
  if (tryStopProcessing())
  {
    m_processingThread.join();
  }
}

namespace {
/// @brief - Defines the time allocated to simulate one tick of
/// the simulation. This value is expressed in milliseconds and
/// currently represent 20 update cycles per second.
constexpr time::Milliseconds SLEEP_DURATION_WHEN_PROCESSING{50};
} // namespace

void EnvironmentProcessor::asyncProcessing()
{
  bool running{true};
  auto lastFrameTimestamp = time::now();

  debug("Started processing for environment");

  while (running)
  {
    std::this_thread::sleep_for(SLEEP_DURATION_WHEN_PROCESSING);

    const auto thisFrameTimestamp = time::now();

    const auto elapsed = time::Duration{
      .unit    = time::Unit::MILLISECONDS,
      .elapsed = time::diffInMs(lastFrameTimestamp, thisFrameTimestamp),
    };

    const auto data = m_timeManager->tick(elapsed);

    m_environment->simulate(data);

    lastFrameTimestamp = thisFrameTimestamp;

    const std::lock_guard guard(m_locker);
    running = m_running;
  }

  debug("Stopped processing for environment");
}

bool EnvironmentProcessor::tryStopProcessing()
{
  const std::lock_guard guard(m_locker);
  if (!m_running)
  {
    return false;
  }

  m_running = false;

  return true;
}

} // namespace swarms::core
