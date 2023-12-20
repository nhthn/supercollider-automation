#include "automation_core.hpp"

double calcAutomation(Automation* automation, double time)
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
            return x1 + (x2 - x1) * (time - cumulativeTime) / duration;
        }
        cumulativeTime = newCumulativeTime;
    }

    return automation->values[numValues - 1];
}