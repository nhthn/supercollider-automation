#include <cmath>
#include <numbers>
#include <algorithm>
#include <functional>

#include "automation_core.hpp"

namespace automation {

EasingFunction::EasingFunction(EasingType type, EasingDirection direction, int parameter)
    : mType(type), mDirection(direction), mParameter(parameter)
{
}

EasingFunction easingFunctionFromInt(int index) {
    if (index == 0) {
        EasingFunction result(EasingType::Linear, EasingDirection::In, 0);
        return result;
    }
    int offset = index - 1;

    if (offset < kNumStandardEasingTypes * 3) {
        int indexIntoStandardEasingTypes = offset / 3;
        auto easingType = kStandardEasingTypes[indexIntoStandardEasingTypes];
        auto easingDirection = kStandardEasingDirections[offset % 3];
        EasingFunction result(easingType, easingDirection, 0);
        return result;
    }

    offset -= kNumStandardEasingTypes * 3;

    for (int i = 0; i < kNumParametrizedEasingTypes; i++) {
        EasingType easingType;
        int rangeMin;
        int rangeMax;
        std::tie(easingType, rangeMin, rangeMax) = kParameterizedEasingTypes[i];
        int rangeSize = rangeMax - rangeMin + 1;
        int numFunctions = rangeSize * 3;
        if (offset < numFunctions) {
            int parameter = rangeMin + offset / 3;
            auto easingDirection = kStandardEasingDirections[offset % 3];
            EasingFunction result(easingType, easingDirection, parameter);
            return result;
        }
        offset -= numFunctions;
    }

    {
        EasingFunction result(EasingType::Linear, EasingDirection::In, 0);
        return result;
    }
}

double coreStep(double t) {
    return 0.0;
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

struct CoreElastic {
    int mNumCrossings = 1;

    CoreElastic(int numCrossings)
    : mNumCrossings(numCrossings) {
        // Empty.
    }

    double operator()(double t) {
        auto coefficient = 5.0;
        // The first term here is a cosine wave which is calibrated have value 0 at time 0, value 1
        // at time 1, and numCrossings zero crossings in the range (0, 1).

        // The second term is exponential growth. It ends at value 1 at time 1, but it doesn't
        // need to start at 0 as the first term does, so we don't need to make the same adjustment
        // as in the PseudoExponential easing function.
        return (
            std::cos((1 - t) * 2 * std::numbers::pi * (mNumCrossings / 2.0 + 1.0 / 4))
            * std::pow(2.0, coefficient * (t - 1))
        );
    }
};

struct CoreSinc {
    int mNumCrossings = 1;

    CoreSinc(int numCrossings)
    : mNumCrossings(numCrossings) {
        // Empty.
    }

    double operator()(double t) {
        auto s = (1 - t) * mNumCrossings * std::numbers::pi;
        if (s < 1e-9) {
            return 1.0;
        }
        return std::sin(s) / s;
    }
};

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
    const auto easingType = easingFunction.mType;
    const auto direction = easingFunction.mDirection;
    if (easingType == EasingType::Linear) {
        return t;
    }
    std::function<double(double)> core;
    if (easingType == EasingType::Step) {
        core = coreStep;
    } else if (easingType == EasingType::Quadratic) {
        core = coreQuadratic;
    } else if (easingType == EasingType::Sine) {
        core = coreSine;
    } else if (easingType == EasingType::Cubic) {
        core = coreCubic;
    } else if (easingType == EasingType::Quartic) {
        core = coreQuartic;
    } else if (easingType == EasingType::Quintic) {
        core = coreQuintic;
    } else if (easingType == EasingType::PseudoExponential) {
        core = corePseudoExponential;
    } else if (easingType == EasingType::Circular) {
        core = coreCircular;
    } else if (easingType == EasingType::Elastic) {
        CoreElastic elastic(easingFunction.mParameter);
        core = elastic;
    } else if (easingType == EasingType::Sinc) {
        CoreSinc sinc(easingFunction.mParameter);
        core = sinc;
    }

    if (direction == EasingDirection::In) {
        return core(t);
    } else if (direction == EasingDirection::Out) {
        return 1 - core(1 - t);
    } else if (direction == EasingDirection::InOut) {
        if (t < 0.5) {
            return core(2 * t) / 2;
        }
        return 1 - core(2 * (1 - t)) / 2;
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