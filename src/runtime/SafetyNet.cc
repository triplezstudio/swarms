
#include "SafetyNet.hh"
#include "CoreException.hh"
#include "Locator.hh"

namespace swarms::runtime {

bool launchProtected(std::function<void(void)> func,
                     const std::string_view module,
                     const std::string &functionName)
{
  try
  {
    func();

    return true;
  }
  catch (const CoreException &e)
  {
    log::Locator::getLogger().error(module,
                                    "Caught exception while executing \"" + functionName + "\"",
                                    e.what());
  }
  catch (const std::exception &e)
  {
    log::Locator::getLogger().error(module,
                                    "Caught unexpected exception while executing \"" + functionName
                                      + "\"",
                                    e.what());
  }
  catch (...)
  {
    log::Locator::getLogger().error(module,
                                    "Unknown error while executing \"" + functionName + "\"");
  }

  return false;
}

bool launchProtected(std::function<void(void)> func,
                     const std::string &functionName,
                     const log::PrefixedLogger &logger)
{
  try
  {
    func();

    return true;
  }
  catch (const CoreException &e)
  {
    logger.error("Caught exception while executing \"" + functionName + "\"", e.what());
  }
  catch (const std::exception &e)
  {
    logger.error("Caught unexpected exception while executing \"" + functionName + "\"", e.what());
  }
  catch (...)
  {
    logger.error("Unknown error while executing \"" + functionName + "\"");
  }

  return false;
}

} // namespace swarms::runtime
