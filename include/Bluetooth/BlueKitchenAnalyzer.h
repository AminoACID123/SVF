#ifndef BLUE_KITCHEN_ANALYZER_H
#define BLUE_KITCHEN_ANALYZER_H
#include "BTAnalyzer.h"

class BlueKitchenAnalyzer : public BTAnalyzer
{

    void analyze();

    void extractHCI();

    void extractAPI();

public:

    BlueKitchenAnalyzer(SVF::SVFModule* m) : BTAnalyzer(m) {}

};

#endif