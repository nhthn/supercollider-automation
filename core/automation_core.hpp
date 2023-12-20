#pragma once

namespace automation {

struct Automation {
    int numValues;
    double* values;
    double* durations;
};

double evaluate(const Automation* automation, double time);
void normalizeDurations(Automation* automation);

}