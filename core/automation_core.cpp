#include <cmath>
#include <numbers>
#include <algorithm>
#include <functional>

#include "automation_core.hpp"

namespace automation {

static constexpr float kPi = static_cast<float>(std::numbers::pi);

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

float coreStep(float t) {
    return 0.0;
}

float coreSine(float t) {
    return 1.0f - std::cosf(t * kPi / 2.0f);
}

float coreQuadratic(float t) {
    return t * t;
}

float coreCubic(float t) {
    return t * t * t;
}

float coreQuartic(float t) {
    return t * t * t * t;
}

float coreQuintic(float t) {
    return t * t * t * t * t;
}

float corePseudoExponential(float t) {
    float coefficient = 10.0f;
    return (std::powf(2.0f, coefficient * t) - 1.0f) / (std::powf(2.0f, coefficient) - 1.0f);
}

float coreCircular(float t) {
    return 1 - std::sqrtf(1 - t * t);
}

struct CoreElastic {
    int mNumCrossings = 1;

    CoreElastic(int numCrossings)
    : mNumCrossings(numCrossings) {
        // Empty.
    }

    float operator()(float t) {
        float coefficient = 5.0;
        // The first term here is a cosine wave which is calibrated have value 0 at time 0, value 1
        // at time 1, and numCrossings zero crossings in the range (0, 1).

        // The second term is exponential growth. It ends at value 1 at time 1, but it doesn't
        // need to start at 0 as the first term does, so we don't need to make the same adjustment
        // as in the PseudoExponential easing function.
        return (
            std::cosf((1.0f - t) * 2.0f * kPi * (mNumCrossings / 2.0f + 1.0f / 4))
            * std::powf(2.0f, coefficient * (t - 1.0f))
        );
    }
};

struct CoreSinc {
    int mNumCrossings = 1;

    CoreSinc(int numCrossings)
    : mNumCrossings(numCrossings) {
        // Empty.
    }

    float operator()(float t) {
        auto s = (1 - t) * mNumCrossings * kPi;
        if (s < 1e-9) {
            return 1.0;
        }
        return std::sin(s) / s;
    }
};

struct CoreStaircase {
    int mNumSteps = 1;

    CoreStaircase(int numSteps)
    : mNumSteps(numSteps) {
        // Empty.
    }

    float operator()(float t) {
        return std::floor(t * mNumSteps) / mNumSteps;
    }
};

float easeOut(float t, std::function<float(float)> core) {
    return 1 - core(1 - t);
}

float easeInOut(float t, std::function<float(float)> core) {
    if (t < 0.5) {
        return core(2 * t) / 2;
    }
    return 1 - core(2 * (1 - t)) / 2;
}

float computeEasingCore(float t, EasingFunction easingFunction) {
    const auto easingType = easingFunction.mType;
    const auto direction = easingFunction.mDirection;
    if (easingType == EasingType::Linear) {
        return t;
    }
    std::function<float(float)> core;
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
        CoreElastic function(easingFunction.mParameter);
        core = function;
    } else if (easingType == EasingType::Sinc) {
        CoreSinc function(easingFunction.mParameter);
        core = function;
    } else if (easingType == EasingType::Staircase) {
        CoreStaircase function(easingFunction.mParameter);
        core = function;
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

float computeEasing(float x1, float x2, float t, EasingFunction easingFunction) {
    auto tWarped = computeEasingCore(t, easingFunction);
    return x1 + (x2 - x1) * tWarped;
}

float evaluate(const Automation* automation, float time)
{
    const int numValues = automation->numValues;
    const int numDurations = numValues - 1;
    if (time <= 0.0) {
        return automation->values[0];
    }

    float cumulativeTime = 0.0;
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
    float sum = 0.0;
    for (int i = 0; i < numDurations; i++) {
        automation->durations[i] = std::max(automation->durations[i], 0.0f);
        sum += automation->durations[i];
    }
    float multiplier = sum == 0.0 ? 1.0f : 1.0f / sum;
    for (int i = 0; i < numDurations; i++) {
        automation->durations[i] *= multiplier;
    }
}

}