// PluginAutomation.cpp
// Nathan Ho (nathan.waikin.ho@gmail.com)

#include "SC_PlugIn.hpp"
#include "Automation.hpp"

static InterfaceTable* ft;

namespace Automation {

Automation::Automation() {
    mCalcFunc = make_calc_function<Automation, &Automation::next>();
    next(1);
}

void Automation::next(int nSamples) {

    // Audio rate input
    const float* input = in(0);

    // Control rate parameter: gain.
    const float gain = in0(1);

    // Output buffer
    float* outbuf = out(0);

    // simple gain function
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = input[i] * gain;
    }
}

} // namespace Automation

PluginLoad(AutomationUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<Automation::Automation>(ft, "Automation", false);
}
