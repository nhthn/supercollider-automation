#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "automation_core.hpp"

using EasingFunction = automation::EasingFunction;
using EasingType = automation::EasingType;
using EasingDirection = automation::EasingDirection;
using Automation = automation::Automation;

const float kEpsilon = 1e-9f;

TEST_CASE("single segment")
{
    int numValues = 2;
    float values[] = {1.0f, 2.0f};
    float durations[] = {2.0f};
    EasingFunction easingFunctions[] = {
        { EasingType::Linear, EasingDirection::In, 0 },
    };
    Automation automation = {
        .numValues = numValues,
        .values = values,
        .durations = durations,
        .easingFunctions = easingFunctions
    };
    REQUIRE_THAT(automation::evaluate(&automation, -1.0f), Catch::Matchers::WithinRel(1.0f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 0.0f), Catch::Matchers::WithinRel(1.0f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 0.5f), Catch::Matchers::WithinRel(1.25f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 1.0f), Catch::Matchers::WithinRel(1.5f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 2.0f), Catch::Matchers::WithinRel(2.0f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 3.0f), Catch::Matchers::WithinRel(2.0f, kEpsilon));
}


TEST_CASE("multiple segments")
{
    int numValues = 3;
    float values[] = {1.0f, 5.0f, -3.0f};
    float durations[] = {2.0f, 1.0f};
    EasingFunction easingFunctions[] = {
        { EasingType::Linear, EasingDirection::In, 0 },
        { EasingType::Linear, EasingDirection::In, 0 },
    };
    Automation automation = {
        .numValues = numValues,
        .values = values,
        .durations = durations,
        .easingFunctions = easingFunctions
    };
    REQUIRE_THAT(automation::evaluate(&automation, -1.0f), Catch::Matchers::WithinRel(1.0f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 0.0f), Catch::Matchers::WithinRel(1.0f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 1.0f), Catch::Matchers::WithinRel(3.0f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 2.0f), Catch::Matchers::WithinRel(5.0f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 3.0f), Catch::Matchers::WithinRel(-3.0f, kEpsilon));
    REQUIRE_THAT(automation::evaluate(&automation, 4.0f), Catch::Matchers::WithinRel(-3.0f, kEpsilon));
}

TEST_CASE("NORMALIZATION")
{
    int numValues = 3;
    float values[] = {1.0f, 2.0f, 1.0f};
    float durations[] = {2.0f, 3.0f};
    EasingFunction easingFunctions[] = {
        { EasingType::Linear, EasingDirection::In, 0 },
        { EasingType::Linear, EasingDirection::In, 0 },
    };
    Automation automation = {
        .numValues = numValues,
        .values = values,
        .durations = durations,
        .easingFunctions = easingFunctions
    };
    normalizeDurations(&automation);
    REQUIRE_THAT(automation.durations[0], Catch::Matchers::WithinRel(2.0f / 5.0f, kEpsilon));
    REQUIRE_THAT(automation.durations[1], Catch::Matchers::WithinRel(3.0f / 5.0f, kEpsilon));
}

TEST_CASE("enumeration")
{
    auto easingFunction = automation::easingFunctionFromInt(25);
    REQUIRE(easingFunction.mType == EasingType::Elastic);
    REQUIRE(easingFunction.mDirection == EasingDirection::In);
    REQUIRE(easingFunction.mParameter == 1);
}