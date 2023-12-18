// PluginAutomation.hpp
// Nathan Ho (nathan.waikin.ho@gmail.com)

#pragma once

#include "SC_PlugIn.hpp"

namespace Automation {

class Automation : public SCUnit {
public:
    Automation();

    // Destructor
    // ~Automation();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace Automation
