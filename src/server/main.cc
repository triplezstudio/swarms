
#include "Locator.hh"
#include "SafetyNet.hh"
#include "Server.hh"
#include "StdLogger.hh"
#include <csignal>
#include <functional>

namespace {
// https://stackoverflow.com/questions/11468414/using-auto-and-lambda-to-handle-signal
std::function<void(int)> sigIntProcessing;
void sigIntInterceptor(const int signal)
{
  sigIntProcessing(signal);
}
} // namespace

int main(int /*argc*/, char ** /*argv*/)
{
  swarms::log::StdLogger raw;
  raw.setLevel(swarms::log::Severity::DEBUG);
  swarms::log::Locator::provide(&raw);

  swarms::server::Server server;

  sigIntProcessing = [&server](const int /*signal*/) { server.requestStop(); };
  // https://en.cppreference.com/w/cpp/utility/program/signal
  std::signal(SIGINT, sigIntInterceptor);

  auto gameFunc = [&server]() { server.run(); };

  if (!swarms::runtime::launchProtected(gameFunc, "main", "gameFunc"))
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
