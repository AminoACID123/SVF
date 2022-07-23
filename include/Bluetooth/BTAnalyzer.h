#ifndef BT_ANALYZER_H
#define BT_ANALYZER_H

#include "Util/SVFModule.h"
#include "SVF-FE/SVFIRBuilder.h"
#include <vector>

class BTAnalyzer
{
public:
    typedef std::vector<const SVF::Function*> FunctionSetType;

private:
    FunctionSetType interfaces;
    SVF::SVFIR* pag;

public:
    BTAnalyzer(SVF::SVFModule* svfModule);

    void printGlobals();

    void printFunctions();

    void printIndirectCalls();

    void extractInterface();

    void analyze(SVF::Function* F);

    SVF::SVFIR* getPag()
    {
        return pag;
    }


};


#endif