#pragma once

#include "SC_PlugIn.hpp"
#include "automation_core.hpp"

using AutomationCore = automation::Automation;

class Automation : public SCUnit {
public:
    Automation();
    ~Automation();

private:
    void next(int nSamples);

    AutomationCore* mCore = nullptr;
};
