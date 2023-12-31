#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "automation_core.hpp"

using Easing = automation::EasingFunction;
using Automation = automation::Automation;

const double kEpsilon = 1e-9;

TEST_CASE("single segment")
{
    int numValues = 2;
    double values[] = {1.0, 2.0};
    double durations[] = {2.0};
    Easing easingFunctions[] = {Easing::Linear, Easing::Linear};
    Automation automation = {
        .numValues = numValues,
        .values = values,
        .durations = durations,
        .easingFunctions = easingFunctions
    };
    REQUIRE_THAT(automation::evaluate(&automation, -1.0), Catch::Matchers::WithinRel(1.0, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 0.0), Catch::Matchers::WithinRel(1.0, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 0.5), Catch::Matchers::WithinRel(1.25, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 1.0), Catch::Matchers::WithinRel(1.5, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 2.0), Catch::Matchers::WithinRel(2.0, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 3.0), Catch::Matchers::WithinRel(2.0, kEpsilon));
}


TEST_CASE("multiple segments")
{
    int numValues = 3;
    double values[] = {1.0, 5.0, -3.0};
    double durations[] = {2.0, 1.0};
    Easing easingFunctions[] = {Easing::Linear, Easing::Linear};
    Automation automation = {
        .numValues = numValues,
        .values = values,
        .durations = durations,
        .easingFunctions = easingFunctions
    };
    REQUIRE_THAT(automation::evaluate(&automation, -1.0), Catch::Matchers::WithinRel(1.0, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 0.0), Catch::Matchers::WithinRel(1.0, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 1.0), Catch::Matchers::WithinRel(3.0, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 2.0), Catch::Matchers::WithinRel(5.0, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 3.0), Catch::Matchers::WithinRel(-3.0, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 4.0), Catch::Matchers::WithinRel(-3.0, kEpsilon));
}

TEST_CASE("normalization")
{
    int numValues = 3;
    double values[] = {1.0, 2.0, 1.0};
    double durations[] = {2.0, 3.0};
    Easing easingFunctions[] = {Easing::Linear, Easing::Linear};
    Automation automation = {
        .numValues = numValues,
        .values = values,
        .durations = durations,
        .easingFunctions = easingFunctions
    };
    normalizeDurations(&automation);
    REQUIRE_THAT(automation.durations[0], Catch::Matchers::WithinRel(2.0 / 5.0, kEpsilon));
    REQUIRE_THAT(automation.durations[1], Catch::Matchers::WithinRel(3.0 / 5.0, kEpsilon));
}