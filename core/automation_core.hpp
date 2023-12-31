#pragma once

namespace automation {

enum class EasingFunction {
    Linear,
    FirstValue,
    NextValue,
    SineIn,
    SineOut,
    QuadraticIn,
    QuadraticOut,
};

struct Automation {
    int numValues;
    double* values;
    double* durations;
    EasingFunction* easingFunctions;
};

double evaluate(const Automation* automation, double time);
void normalizeDurations(Automation* automation);

}