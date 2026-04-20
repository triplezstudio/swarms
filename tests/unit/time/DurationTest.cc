
#include "Duration.hh"
#include <algorithm>
#include <gtest/gtest.h>

using namespace ::testing;

namespace swarms::time {

struct TestCaseDurationConversion
{
  Duration in{};
  Unit unit{};
  double expected{};
};

using DurationTest = TestWithParam<TestCaseDurationConversion>;

TEST_P(DurationTest, ConvertToUnit)
{
  const auto &param = GetParam();

  const auto actual = param.in.convert(param.unit);

  EXPECT_EQ(actual.unit, param.unit);
  EXPECT_FLOAT_EQ(actual.elapsed, param.expected);
}

INSTANTIATE_TEST_SUITE_P(
  Unit_Time_Duration,
  DurationTest,
  Values(
    // Seconds to seconds
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::SECONDS, .elapsed = 1.23},
                               .unit     = Unit::SECONDS,
                               .expected = 1.23},
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::SECONDS, .elapsed = 9879.236},
                               .unit     = Unit::SECONDS,
                               .expected = 9879.236},
    // Milliseconds to seconds
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::MILLISECONDS, .elapsed = 1000.0},
                               .unit     = Unit::SECONDS,
                               .expected = 1.0},
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::MILLISECONDS, .elapsed = 123.0},
                               .unit     = Unit::SECONDS,
                               .expected = 0.123},
    TestCaseDurationConversion{.in   = Duration{.unit = Unit::MILLISECONDS, .elapsed = 9879.236},
                               .unit = Unit::SECONDS,
                               .expected = 9.879236},
    // Seconds to milliseconds
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::SECONDS, .elapsed = 9879.236},
                               .unit     = Unit::MILLISECONDS,
                               .expected = 9879236.0},
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::SECONDS, .elapsed = 12.2367},
                               .unit     = Unit::MILLISECONDS,
                               .expected = 12236.7},
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::SECONDS, .elapsed = 1.0},
                               .unit     = Unit::MILLISECONDS,
                               .expected = 1000.0},
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::SECONDS, .elapsed = 0.9},
                               .unit     = Unit::MILLISECONDS,
                               .expected = 900.0},
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::SECONDS, .elapsed = 0.0001},
                               .unit     = Unit::MILLISECONDS,
                               .expected = 0.1},
    // Milliseconds to milliseconds
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::MILLISECONDS, .elapsed = 543.217},
                               .unit     = Unit::MILLISECONDS,
                               .expected = 543.217},
    TestCaseDurationConversion{.in       = Duration{.unit = Unit::MILLISECONDS, .elapsed = 0.325},
                               .unit     = Unit::MILLISECONDS,
                               .expected = 0.325}),
  [](const TestParamInfo<TestCaseDurationConversion> &info) -> std::string {
    auto out = std::to_string(info.param.expected) + asTimeString(info.param.unit);
    std::replace(out.begin(), out.end(), '.', '_');
    return out;
  });

} // namespace swarms::time
