#pragma once

namespace automation {

enum class EasingFunction {
    Linear,
    FirstValue,
    NextValue,
    QuadraticIn,
    QuadraticOut,
    QuadraticInOut,
    SineIn,
    SineOut,
    SineInOut,
    CubicIn,
    CubicOut,
    CubicInOut,
    QuarticIn,
    QuarticOut,
    QuarticInOut,
    QuinticIn,
    QuinticOut,
    QuinticInOut,
};

struct Automation {
    int numValues;
    double* values;
    double* durations;
    EasingFunction* easingFunctions;
};

double evaluate(const Automation* automation, double time);
void normalizeDurations(Automation* automation);
EasingFunction easingFunctionFromInt(int index);

}