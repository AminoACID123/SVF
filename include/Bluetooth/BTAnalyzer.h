#ifndef BT_ANALYZER_H
#define BT_ANALYZER_H

#include "Util/SVFModule.h"
#include "SVF-FE/SVFIRBuilder.h"
#include <vector>
#include <map>
#include <queue>

class BTAnalyzer
{
public:
    typedef std::vector<SVF::Function*> FunctionSetType;
    typedef std::queue<SVF::Function*> FunctionWorkList;

private:
    FunctionSetType interfaces;         // Interface functions provided for upper application
    FunctionSetType io_send;            // Functions used to send data to the controller
    FunctionSetType io_recv;            // Functions used to recv date from the controller

    SVF::ICFG* icfg;
    SVF::SVFIR* pag;
    SVF::PTACallGraph* callgraph;
    
public:
    BTAnalyzer(SVF::SVFModule* svfModule);

    void analyze();

    void printGlobals();

    void printFunctions();

    void printIndirectCalls();

    void extractInterface();

    void analyze(SVF::Function* F);

    inline SVF::SVFIR* getPag()
    {
        return pag;
    }

};


#endif