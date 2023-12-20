struct Automation {
    int numValues;
    double* values;
    double* durations;
};

double calcAutomation(const Automation* automation, double time);
void normalizeDurations(Automation* automation);