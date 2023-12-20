struct Automation {
    int numValues;
    double* values;
    double* durations;
};

double calcAutomation(Automation* automation, double time);