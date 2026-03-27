
#include "TimeManager.hh"
#include <gtest/gtest.h>

using namespace ::testing;

namespace swarms::time {

struct TestCaseTick
{
  int id{};
  Tick startTick{};
  TimeStep step{};
  Duration elapsed{};
  Tick expectedTick{};
};

using TickTest = TestWithParam<TestCaseTick>;

TEST_P(TickTest, ReturnsCorrectTick)
{
  const auto &param = GetParam();

  TimeManager manager(param.startTick, param.step);

  const auto actual = manager.tick(param.elapsed);

  EXPECT_DOUBLE_EQ(actual.tick, param.expectedTick);
}

INSTANTIATE_TEST_SUITE_P(Unit_Time_TimeManager,
                         TickTest,
                         Values(
                           TestCaseTick{
                             .id           = 0,
                             .startTick    = Tick(),
                             .step         = TimeStep{},
                             .elapsed      = Duration{.unit = Unit::SECONDS, .elapsed = 1.0},
                             .expectedTick = Tick(10.0),
                           },
                           TestCaseTick{
                             .id        = 1,
                             .startTick = Tick(),
                             .step      = TimeStep{},
                             .elapsed   = Duration{.unit = Unit::MILLISECONDS, .elapsed = 1000.0},
                             .expectedTick = Tick(10.0),
                           },
                           TestCaseTick{
                             .id           = 2,
                             .startTick    = Tick(),
                             .step         = TimeStep{},
                             .elapsed      = Duration{.unit = Unit::MILLISECONDS, .elapsed = 128.0},
                             .expectedTick = Tick(1.28),
                           },
                           TestCaseTick{
                             .id           = 3,
                             .startTick    = Tick(14.04),
                             .step         = TimeStep{},
                             .elapsed      = Duration{.unit = Unit::MILLISECONDS, .elapsed = 128.0},
                             .expectedTick = Tick(15.32),
                           }),
                         [](const TestParamInfo<TestCaseTick> &info) -> std::string {
                           return std::to_string(info.param.id);
                         });

struct TestCaseElapsed
{
  int id{};
  Tick startTick{};
  TimeStep step{};
  Duration elapsed{};
  TickDuration expectedElapsed{};
};

using ElapsedTest = TestWithParam<TestCaseElapsed>;

TEST_P(ElapsedTest, ReturnsCorrectElapsed)
{
  const auto &param = GetParam();

  TimeManager manager(param.startTick, param.step);

  const auto actual = manager.tick(param.elapsed);

  EXPECT_EQ(actual.elapsed, param.expectedElapsed)
    << "Expectd elapsed " << actual.elapsed << " to equal " << param.expectedElapsed;
}

INSTANTIATE_TEST_SUITE_P(Unit_Time_TimeManager,
                         ElapsedTest,
                         Values(
                           TestCaseElapsed{
                             .id              = 0,
                             .startTick       = Tick(),
                             .step            = TimeStep{},
                             .elapsed         = Duration{.unit = Unit::SECONDS, .elapsed = 1.0},
                             .expectedElapsed = TickDuration{10.0},
                           },
                           TestCaseElapsed{
                             .id        = 1,
                             .startTick = Tick(),
                             .step      = TimeStep{},
                             .elapsed   = Duration{.unit = Unit::MILLISECONDS, .elapsed = 1000.0},
                             .expectedElapsed = TickDuration{10.0},
                           },
                           TestCaseElapsed{
                             .id        = 2,
                             .startTick = Tick(),
                             .step      = TimeStep{},
                             .elapsed   = Duration{.unit = Unit::MILLISECONDS, .elapsed = 128.0},
                             .expectedElapsed = TickDuration{1.28},
                           },
                           TestCaseElapsed{
                             .id        = 3,
                             .startTick = Tick(14.04),
                             .step      = TimeStep{},
                             .elapsed   = Duration{.unit = Unit::MILLISECONDS, .elapsed = 128.0},
                             .expectedElapsed = TickDuration{1.28},
                           }),
                         [](const TestParamInfo<TestCaseElapsed> &info) -> std::string {
                           return std::to_string(info.param.id);
                         });

} // namespace swarms::time
