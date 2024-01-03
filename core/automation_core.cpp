#include <cmath>
#include <numbers>
#include <algorithm>

#include "automation_core.hpp"

namespace automation {

EasingFunction easingFunctionFromInt(int index) {
    switch (index) {
    case 0: return EasingFunction::Linear;
    case 1: return EasingFunction::FirstValue;
    case 2: return EasingFunction::NextValue;
    case 3: return EasingFunction::QuadraticIn;
    case 4: return EasingFunction::QuadraticOut;
    case 5: return EasingFunction::QuadraticInOut;
    case 6: return EasingFunction::CubicIn;
    case 7: return EasingFunction::CubicOut;
    case 8: return EasingFunction::CubicInOut;
    case 9: return EasingFunction::SineIn;
    case 10: return EasingFunction::SineOut;
    case 11: return EasingFunction::SineInOut;
    default: return EasingFunction::Linear;
    }
}

double computeEasingCore(double t, EasingFunction easingFunction) {
    if (easingFunction == EasingFunction::Linear) {
        return t;
    }
    if (easingFunction == EasingFunction::FirstValue) {
        return 0.0;
    }
    if (easingFunction == EasingFunction::NextValue) {
        return 1.0;
    }

    if (easingFunction == EasingFunction::QuadraticIn) {
        return t * t;
    }
    if (easingFunction == EasingFunction::QuadraticOut) {
        return 1 - computeEasingCore(1 - t, EasingFunction::QuadraticIn);
    }
    if (easingFunction == EasingFunction::QuadraticInOut) {
        if (t < 0.5) {
            return computeEasingCore(2 * t, EasingFunction::QuadraticOut) / 2;
        }
        if (t >= 0.5) {
            return 1 - computeEasingCore(2 * (1 - t), EasingFunction::QuadraticOut) / 2;
        }
    }

    if (easingFunction == EasingFunction::CubicIn) {
        return t * t * t;
    }
    if (easingFunction == EasingFunction::CubicOut) {
        return 1 - computeEasingCore(1 - t, EasingFunction::CubicIn);
    }
    if (easingFunction == EasingFunction::CubicInOut) {
        if (t < 0.5) {
            return computeEasingCore(2 * t, EasingFunction::CubicIn) / 2;
        }
        if (t >= 0.5) {
            return 1 - computeEasingCore(2 * (1 - t), EasingFunction::CubicIn) / 2;
        }
    }

    if (easingFunction == EasingFunction::SineIn) {
        return 1 - std::cos(t * std::numbers::pi / 2);
    }
    if (easingFunction == EasingFunction::SineOut) {
        return 1 - computeEasingCore(1 - t, EasingFunction::SineIn);
    }
    if (easingFunction == EasingFunction::SineInOut) {
        if (t < 0.5) {
            return computeEasingCore(2 * t, EasingFunction::SineIn) / 2;
        }
        if (t >= 0.5) {
            return 1 - computeEasingCore(2 * (1 - t), EasingFunction::SineIn) / 2;
        }
    }

    return t;
}

double computeEasing(double x1, double x2, double t, EasingFunction easingFunction) {
    auto tWarped = computeEasingCore(t, easingFunction);
    return x1 + (x2 - x1) * tWarped;
}

double evaluate(const Automation* automation, double time)
{
    const int numValues = automation->numValues;
    const int numDurations = numValues - 1;
    if (time <= 0.0) {
        return automation->values[0];
    }

    double cumulativeTime = 0.0;
    for (int i = 0; i < numDurations; i++) {
        const auto duration = automation->durations[i];
        const auto newCumulativeTime = cumulativeTime + duration;
        if (newCumulativeTime >= time) {
            auto x1 = automation->values[i];
            auto x2 = automation->values[i + 1];
            auto easingFunction = automation->easingFunctions[i];
            auto t = (time - cumulativeTime) / duration;
            return computeEasing(x1, x2, t, easingFunction);
        }
        cumulativeTime = newCumulativeTime;
    }

    return automation->values[numValues - 1];
}

void normalizeDurations(Automation* automation)
{
    const int numDurations = automation->numValues - 1;
    double sum = 0.0;
    for (int i = 0; i < numDurations; i++) {
        automation->durations[i] = std::max(automation->durations[i], 0.0);
        sum += automation->durations[i];
    }
    double multiplier = sum == 0.0 ? 1.0 : 1.0 / sum;
    for (int i = 0; i < numDurations; i++) {
        automation->durations[i] *= multiplier;
    }
}

}