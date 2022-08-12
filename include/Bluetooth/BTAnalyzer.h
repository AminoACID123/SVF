#ifndef BT_ANALYZER_H
#define BT_ANALYZER_H

#include "Util/SVFModule.h"
#include "SVF-FE/SVFIRBuilder.h"
#include "Graphs/SVFG.h"
#include <vector>
#include <map>
#include <queue>
#include <set>

#define HCI_COMMAND_DATA_PACKET 0x01
#define HCI_ACL_DATA_PACKET     0x02
#define HCI_SCO_DATA_PACKET     0x03
#define HCI_EVENT_PACKET        0x04
#define HCI_ISO_DATA_PACKET     0x05

class BTAnalyzer
{
public:
    typedef std::vector<SVF::Function*> FunctionSetType;
    typedef std::queue<SVF::Function*> FunctionWorkList;
    typedef std::set<int> OpcodeList;

protected:

    /// Bluetooth interfaces
    FunctionSetType interfaces;         // Interface functions provided for upper application
    FunctionSetType hci_send;            // Functions used to send data to the controller
    FunctionSetType hci_recv;            // Functions used to recv data from the controller
    OpcodeList hci_cmds;
    OpcodeList hci_evts;

    /// Summary Infos
    SVF::SVFModule* svfM;
    SVF::Module* llvmM;
    SVF::ICFG* icfg;
    SVF::SVFIR* pag;
    SVF::PTACallGraph* callgraph;
    SVF::SVFG* svfg;

    virtual void initialize();

    virtual void analyze() = 0;

    virtual void extractHCI() = 0;

    virtual void extractAPI() = 0;

    virtual void buildSummaryInfo();
    
public:

    BTAnalyzer(SVF::SVFModule*);

    void run();

    void printGlobals();

    void printFunctions();

    void printIndirectCalls();

};

#endif