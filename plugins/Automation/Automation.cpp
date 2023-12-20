#include "SC_PlugIn.hpp"
#include "Automation.hpp"
#include "automation_core.hpp"

static InterfaceTable* ft;

Automation::Automation() {
    int numValues = static_cast<int>(in0(1));

    mCore = static_cast<AutomationCore*>(RTAlloc(mWorld, sizeof(Automation)));
    mCore->numValues = numValues;
    mCore->values = static_cast<double*>(RTAlloc(mWorld, numValues * sizeof(double)));
    mCore->durations = static_cast<double*>(RTAlloc(mWorld, (numValues - 1) * sizeof(double)));

    mCalcFunc = make_calc_function<Automation, &Automation::next>();
    next(1);
}

Automation::~Automation() {
    RTFree(mWorld, mCore->values);
    RTFree(mWorld, mCore->durations);
    RTFree(mWorld, mCore);
}

void Automation::next(int nSamples) {
    const float* time = in(0);

    float* outBuf = out(0);

    int numValues = mCore->numValues;
    int numDurations = mCore->numValues - 1;

    for (int i = 0; i < nSamples; i++) {
        int inputIndex = 2;
        for (int j = 0; j < numValues; j++) {
            mCore->values[j] = in(inputIndex)[i];
            inputIndex++;
        }
        for (int j = 0; j < numDurations; j++) {
            mCore->durations[j] = in(inputIndex)[i];
            inputIndex++;
        }
        automation::normalizeDurations(mCore);
        outBuf[i] = static_cast<float>(automation::evaluate(mCore, time[i]));
    }
}

PluginLoad(AutomationUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<Automation>(ft, "Automation", false);
}
