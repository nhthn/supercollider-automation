#include "SC_PlugIn.hpp"
#include "Automation.hpp"
#include "automation_core.hpp"

static InterfaceTable* ft;

Automation::Automation() {
    int numValues = static_cast<int>(in0(1));
    if (numValues < 2) {
        mCalcFunc = make_calc_function<Automation, &Automation::clear>();
        mOK = false;
        next(1);
        return;
    }
    int numSegments = numValues - 1;

    auto coreMemory = RTAlloc(mWorld, sizeof(Automation));
    auto valuesMemory = RTAlloc(mWorld, numValues * sizeof(float));
    auto durationsMemory = RTAlloc(mWorld, numSegments * sizeof(float));
    auto easingFunctionMemory = RTAlloc(mWorld, numSegments * sizeof(automation::EasingFunction));

    if (
        coreMemory == nullptr
        || valuesMemory == nullptr
        || durationsMemory == nullptr
        || easingFunctionMemory == nullptr
    ) {
        mOK = false;
        mCalcFunc = make_calc_function<Automation, &Automation::clear>();
        next(1);
        return;
    }
    mOK = true;

    mCore = static_cast<AutomationCore*>(coreMemory);
    mCore->numValues = numValues;
    mCore->values = static_cast<float*>(valuesMemory);
    mCore->durations = static_cast<float*>(durationsMemory);
    mCore->easingFunctions = static_cast<automation::EasingFunction*>(easingFunctionMemory);

    mCalcFunc = make_calc_function<Automation, &Automation::next>();
    next(1);
}

Automation::~Automation() {
    if (mCore != nullptr) {
        RTFree(mWorld, mCore);
        if (mCore->values != nullptr) {
            RTFree(mWorld, mCore->values);
        }
        if (mCore->durations != nullptr) {
            RTFree(mWorld, mCore->durations);
        }
        if (mCore->easingFunctions != nullptr) {
            RTFree(mWorld, mCore->easingFunctions);
        }
    }
}

void Automation::next(int nSamples) {
    const float* time = in(0);

    float* outBuf = out(0);

    int numValues = mCore->numValues;
    int numSegments = mCore->numValues - 1;

    // 2: 1 for the time parameter, one for the number of values.
    // numValues: values.
    // numSegments: durations.
    // numSegments: easing functions.
    int expectedNumInputs = 2 + numValues + numSegments + numSegments;

    if (mNumInputs != expectedNumInputs) {
        clear(nSamples);
        return;
    }

    for (int i = 0; i < nSamples; i++) {
        int inputIndex = 2;
        for (int j = 0; j < numValues; j++) {
            mCore->values[j] = in(inputIndex)[i];
            inputIndex++;
        }
        for (int j = 0; j < numSegments; j++) {
            mCore->durations[j] = in(inputIndex)[i];
            inputIndex++;
        }
        for (int j = 0; j < numSegments; j++) {
            mCore->easingFunctions[j] = automation::easingFunctionFromInt(
                static_cast<int>(in(inputIndex)[i])
            );
            inputIndex++;
        }
        automation::normalizeDurations(mCore);
        outBuf[i] = static_cast<float>(automation::evaluate(mCore, time[i]));
    }
}

void Automation::clear(int nSamples) {
    float* outBuf = out(0);
    for (int i = 0; i < nSamples; i++) {
        outBuf[i] = 0.0f;
    }
}

PluginLoad(AutomationUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<Automation>(ft, "Automation", false);
}
