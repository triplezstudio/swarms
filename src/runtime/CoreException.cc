
#include "CoreException.hh"
#include "Locator.hh"
#include <execinfo.h>
#include <vector>

namespace swarms::runtime {
namespace {
constexpr auto MAX_STACK_TRACE_DEPTH = 32u;

auto retrieveStackTrace() -> std::string
{
  std::vector<void *> addresses(MAX_STACK_TRACE_DEPTH);
  const auto size = backtrace(addresses.data(), addresses.size());
  addresses.resize(size);

  const auto funcs = backtrace_symbols(addresses.data(), addresses.size());

  std::string stackTrace{};
  for (auto i = 0u; i < addresses.size(); ++i)
  {
    stackTrace += " at ";
    stackTrace += funcs[i];
    stackTrace += "\n";
  }

  // https://man7.org/linux/man-pages/man3/backtrace.3.html
  free(funcs);

  return stackTrace;
}
} // namespace

CoreException::CoreException(const std::string &message,
                             const std::optional<std::string> &cause) noexcept
  : std::exception()
  , m_message(message)
{
  log::Locator::getLogger().error(message, cause);
  log::Locator::getLogger().error(retrieveStackTrace());
}

CoreException::CoreException(const std::string &message, const CoreException &cause) noexcept
  : std::exception()
  , m_message(message)
{
  log::Locator::getLogger().error(message, cause.what());
  log::Locator::getLogger().error(retrieveStackTrace());
}

auto CoreException::what() const throw() -> const char *
{
  return m_message.c_str();
}

} // namespace swarms::runtime
