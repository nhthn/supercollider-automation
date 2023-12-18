#include <vector>

class Automation {
public:
    Automation(int numBreakpoints);
    void setBreakpointValue(int index, double value);
    void setBreakpointDuration(int index, double duration);
    double valueAt(double time);

private:
    std::vector<double> mDurations;
    std::vector<double> mValues;
    int mNumBreakpoints;
};