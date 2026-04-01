
#include "SafetyNet.hh"
#include "CoreException.hh"
#include "Locator.hh"

namespace swarms::runtime {

bool launchProtected(std::function<void(void)> func, const std::string &functionName)
{
  try
  {
    func();

    return true;
  }
  catch (const CoreException &e)
  {
    log::Locator::getLogger().error("Caught exception while executing \"" + functionName + "\"",
                                    e.what());
  }
  catch (const std::exception &e)
  {
    log::Locator::getLogger().error("Caught unexpected exception while executing \"" + functionName
                                      + "\"",
                                    e.what());
  }
  catch (...)
  {
    log::Locator::getLogger().error("Unknown error while executing \"" + functionName + "\"");
  }

  return false;
}

} // namespace swarms::runtime
