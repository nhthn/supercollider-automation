#include <algorithm>

#include "automation_core.hpp"

double calcAutomation(const Automation* automation, double time)
{
    const int numValues = automation->numValues;
    const int numDurations = numValues - 1;
    if (time <= 0.0) {
        return automation->values[0];
    }

    double cumulativeTime = 0.0;
    for (int i = 0; i < numDurations; i++) {
        const auto duration = std::max(automation->durations[i], 0.0);
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