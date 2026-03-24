
#include "Server.hh"
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
  swarms::Server server;

  sigIntProcessing = [&server](const int /*signal*/) { server.requestStop(); };
  // https://en.cppreference.com/w/cpp/utility/program/signal
  std::signal(SIGINT, sigIntInterceptor);

  server.run();

  return EXIT_SUCCESS;
}
