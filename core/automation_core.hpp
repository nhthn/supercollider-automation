#pragma once

namespace automation {

enum class EasingDirection {
    In,
    Out,
    InOut
};

enum class EasingType {
    Linear,
    Step,
    Quadratic,
    Sine,
    Cubic,
    Quartic,
    Quintic,
    PseudoExponential,
    Circular,
    Elastic,
    Sinc,
    Staircase,
    Bounce,
};

class EasingFunction {
public:
    EasingFunction(EasingType type, EasingDirection direction, int parameter);

    EasingType mType;
    EasingDirection mDirection;
    int mParameter;
};

const EasingDirection kStandardEasingDirections[3] = {
    EasingDirection::In,
    EasingDirection::Out,
    EasingDirection::InOut,
};

constexpr int kNumStandardEasingTypes = 8;
const EasingType kStandardEasingTypes[kNumStandardEasingTypes] = {
    EasingType::Step,
    EasingType::Quadratic,
    EasingType::Sine,
    EasingType::Cubic,
    EasingType::Quartic,
    EasingType::Quintic,
    EasingType::PseudoExponential,
    EasingType::Circular,
};

constexpr int kNumParametrizedEasingTypes = 4;
const std::tuple<EasingType, int, int> kParameterizedEasingTypes[kNumParametrizedEasingTypes] = {
    { EasingType::Elastic, 1, 8 },
    { EasingType::Sinc, 1, 8 },
    { EasingType::Staircase, 2, 8 },
    { EasingType::Bounce, 1, 8 }
};

struct Automation {
    int numValues;
    float* values;
    float* durations;
    EasingFunction* easingFunctions;
};

float evaluate(const Automation* automation, float time);
void normalizeDurations(Automation* automation);
EasingFunction easingFunctionFromInt(int index);

}