#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "automation_core.hpp"

const double kEpsilon = 1e-9;

TEST_CASE("single segment")
{
    int numBreakpoints = 2;
    Automation automation(numBreakpoints);
    automation.setBreakpointValue(0, 1.0);
    automation.setBreakpointValue(1, 2.0);
    automation.setBreakpointDuration(0, 2.0);
    REQUIRE_THAT(automation.valueAt(-1.0), Catch::Matchers::WithinRel(1.0, kEpsilon));
    REQUIRE_THAT(automation.valueAt(0.0), Catch::Matchers::WithinRel(1.0, kEpsilon));
    REQUIRE_THAT(automation.valueAt(0.5), Catch::Matchers::WithinRel(1.25, kEpsilon));
    REQUIRE_THAT(automation.valueAt(2.0), Catch::Matchers::WithinRel(2.0, kEpsilon));
    REQUIRE_THAT(automation.valueAt(3.0), Catch::Matchers::WithinRel(2.0, kEpsilon));
}