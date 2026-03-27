
#include "TimeStep.hh"
#include <gtest/gtest.h>

using namespace ::testing;

namespace swarms::time {

struct TestCaseTimeStepCount
{
  int id{};
  int ticks{};
  Duration duration{};
  Duration input{};
  TickDuration expectedDuration{};
};

using CountTest = TestWithParam<TestCaseTimeStepCount>;

TEST_P(CountTest, CountsCorrectly)
{
  const auto &param = GetParam();

  TimeStep step(param.ticks, param.duration);
  const auto actual = step.count(param.input);

  EXPECT_DOUBLE_EQ(actual, param.expectedDuration);
}

INSTANTIATE_TEST_SUITE_P(Unit_Time_TimeStep,
                         CountTest,
                         Values(
                           TestCaseTimeStepCount{
                             .id               = 0,
                             .ticks            = 1,
                             .duration         = Duration(Unit::MILLISECONDS, 100.0),
                             .input            = Duration(Unit::MILLISECONDS, 100.0),
                             .expectedDuration = TickDuration{1.0},
                           },
                           TestCaseTimeStepCount{
                             .id               = 1,
                             .ticks            = 2,
                             .duration         = Duration(Unit::MILLISECONDS, 100.0),
                             .input            = Duration(Unit::MILLISECONDS, 100.0),
                             .expectedDuration = TickDuration{2.0},
                           },
                           TestCaseTimeStepCount{
                             .id               = 2,
                             .ticks            = 1,
                             .duration         = Duration(Unit::MILLISECONDS, 200.0),
                             .input            = Duration(Unit::MILLISECONDS, 100.0),
                             .expectedDuration = TickDuration{0.5},
                           },
                           TestCaseTimeStepCount{
                             .id               = 3,
                             .ticks            = 2,
                             .duration         = Duration(Unit::SECONDS, 1.0),
                             .input            = Duration(Unit::MILLISECONDS, 100.0),
                             .expectedDuration = TickDuration{0.2},
                           },
                           TestCaseTimeStepCount{
                             .id               = 4,
                             .ticks            = 1,
                             .duration         = Duration(Unit::SECONDS, 2.0),
                             .input            = Duration(Unit::MILLISECONDS, 100.0),
                             .expectedDuration = TickDuration{0.05},
                           },
                           TestCaseTimeStepCount{
                             .id               = 5,
                             .ticks            = 2,
                             .duration         = Duration(Unit::SECONDS, 1.0),
                             .input            = Duration(Unit::SECONDS, 25.0),
                             .expectedDuration = TickDuration{50.0},
                           },
                           TestCaseTimeStepCount{
                             .id               = 6,
                             .ticks            = 1,
                             .duration         = Duration(Unit::MILLISECONDS, 100.0),
                             .input            = Duration(Unit::MILLISECONDS, 250.0),
                             .expectedDuration = TickDuration{2.5},
                           },
                           TestCaseTimeStepCount{
                             .id               = 7,
                             .ticks            = 1,
                             .duration         = Duration(Unit::SECONDS, 0.125),
                             .input            = Duration(Unit::MILLISECONDS, 250.0),
                             .expectedDuration = TickDuration{2.0},
                           }),
                         [](const TestParamInfo<TestCaseTimeStepCount> &info) -> std::string {
                           return std::to_string(info.param.id);
                         });

TEST(Unit_Time_TimeStep, EqualityTest)
{
  TimeStep lhs(1.2, Duration::fromSeconds(1.0));
  TimeStep rhs(1.2, Duration::fromSeconds(1.0));
  EXPECT_EQ(lhs, rhs);

  lhs = TimeStep(1.2, Duration::fromSeconds(1.0));
  rhs = TimeStep(1.2, Duration::fromMilliseconds(1000.0));
  EXPECT_NE(lhs, rhs);

  lhs = TimeStep(14.17, Duration::fromSeconds(1.0));
  rhs = TimeStep(1.2, Duration::fromSeconds(1.0));
  EXPECT_NE(lhs, rhs);

  lhs = TimeStep(21.057, Duration::fromSeconds(1.0));
  rhs = TimeStep(21.057, Duration::fromSeconds(1.1));
  EXPECT_NE(lhs, rhs);

  lhs = TimeStep(1.2, Duration::fromMilliseconds(18.05));
  rhs = TimeStep(1.2, Duration::fromMilliseconds(18.05));
  EXPECT_EQ(lhs, rhs);
}

} // namespace swarms::time
