#include "automation_core.hpp"

Automation::Automation(int numBreakpoints)
: mNumBreakpoints(numBreakpoints)
{
    for (int i = 0; i < mNumBreakpoints - 1; i++) {
        mDurations.push_back(0.0);
    }
    for (int i = 0; i < mNumBreakpoints; i++) {
        mValues.push_back(0.0);
    }
}

void Automation::setBreakpointDuration(int index, double duration)
{
    if ((0 <= index) && (index <= mNumBreakpoints - 1)) {
        mDurations[index] = duration;
    }
}

void Automation::setBreakpointValue(int index, double value)
{
    if ((0 <= index) && (index <= mNumBreakpoints)) {
        mValues[index] = value;
    }
}

double Automation::valueAt(double time)
{
    if (time < 0.0) {
        return mValues[0];
    }
    if (time >= mDurations[0]) {
        return mValues[1];
    }
    return mValues[0] + (mValues[1] - mValues[0]) * time / mDurations[0];
}