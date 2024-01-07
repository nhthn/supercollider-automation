#include <cmath>
#include <numbers>
#include <algorithm>
#include <functional>

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
    case 12: return EasingFunction::QuarticIn;
    case 13: return EasingFunction::QuarticOut;
    case 14: return EasingFunction::QuarticInOut;
    case 15: return EasingFunction::QuinticIn;
    case 16: return EasingFunction::QuinticOut;
    case 17: return EasingFunction::QuinticInOut;
    case 18: return EasingFunction::PseudoExponentialIn;
    case 19: return EasingFunction::PseudoExponentialOut;
    case 20: return EasingFunction::PseudoExponentialInOut;
    case 21: return EasingFunction::CircularIn;
    case 22: return EasingFunction::CircularOut;
    case 23: return EasingFunction::CircularInOut;
    default: return EasingFunction::Linear;
    }
}

double coreSine(double t) {
    return 1 - std::cos(t * std::numbers::pi / 2);
}

double coreQuadratic(double t) {
    return t * t;
}

double coreCubic(double t) {
    return t * t * t;
}

double coreQuartic(double t) {
    return t * t * t * t;
}

double coreQuintic(double t) {
    return t * t * t * t * t;
}

double corePseudoExponential(double t) {
    auto coefficient = 10.0;
    return (std::pow(2.0, coefficient * t) - 1.0) / (std::pow(2.0, coefficient) - 1.0);
}

double coreCircular(double t) {
    return 1 - std::sqrt(1 - t * t);
}


double easeOut(double t, std::function<double(double)> core) {
    return 1 - core(1 - t);
}

double easeInOut(double t, std::function<double(double)> core) {
    if (t < 0.5) {
        return core(2 * t) / 2;
    }
    return 1 - core(2 * (1 - t)) / 2;
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

    if (easingFunction == EasingFunction::QuadraticIn) { return coreQuadratic(t); }
    if (easingFunction == EasingFunction::QuadraticOut) { return easeOut(t, coreQuadratic); }
    if (easingFunction == EasingFunction::QuadraticInOut) { return easeInOut(t, coreQuadratic); }

    if (easingFunction == EasingFunction::SineIn) { return coreSine(t); }
    if (easingFunction == EasingFunction::SineOut) { return easeOut(t, coreSine); }
    if (easingFunction == EasingFunction::SineInOut) { return easeInOut(t, coreSine); }

    if (easingFunction == EasingFunction::CubicIn) { return coreCubic(t); }
    if (easingFunction == EasingFunction::CubicOut) { return easeOut(t, coreCubic); }
    if (easingFunction == EasingFunction::CubicInOut) { return easeInOut(t, coreCubic); }

    if (easingFunction == EasingFunction::QuarticIn) { return coreQuartic(t); }
    if (easingFunction == EasingFunction::QuarticOut) { return easeOut(t, coreQuartic); }
    if (easingFunction == EasingFunction::QuarticInOut) { return easeInOut(t, coreQuartic); }

    if (easingFunction == EasingFunction::QuinticIn) { return coreQuintic(t); }
    if (easingFunction == EasingFunction::QuinticOut) { return easeOut(t, coreQuintic); }
    if (easingFunction == EasingFunction::QuinticInOut) { return easeInOut(t, coreQuintic); }

    if (easingFunction == EasingFunction::PseudoExponentialIn) { return corePseudoExponential(t); }
    if (easingFunction == EasingFunction::PseudoExponentialOut) { return easeOut(t, corePseudoExponential); }
    if (easingFunction == EasingFunction::PseudoExponentialInOut) { return easeInOut(t, corePseudoExponential); }

    if (easingFunction == EasingFunction::CircularIn) { return coreCircular(t); }
    if (easingFunction == EasingFunction::CircularOut) { return easeOut(t, coreCircular); }
    if (easingFunction == EasingFunction::CircularInOut) { return easeInOut(t, coreCircular); }

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