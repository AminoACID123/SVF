#ifndef BLUEZ_ANALYZER_H
#define BLUEZ_ANALYZER_H
#include "BTAnalyzer.h"

class BlueZAnalyzer : public BTAnalyzer{
    
    void extractAPI();

    void extractHCI();

public:

    BlueZAnalyzer(SVF::SVFModule* m) : BTAnalyzer(m) {}

    void analyze() {}

};
#endif