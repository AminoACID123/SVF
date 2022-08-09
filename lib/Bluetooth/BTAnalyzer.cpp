#include "Bluetooth/BTAnalyzer.h"
#include "SVF-FE/SVFIRBuilder.h"
#include "Util/Options.h"
#include "Util/SVFUtil.h"
#include "Util/Options.h"
#include "WPA/Andersen.h"
#include <iostream>

using namespace SVF;

BTAnalyzer::BTAnalyzer(SVF::SVFModule* m)
{
    assert(m != nullptr && "SVFModule must be built first.");
    svfM = m;
    llvmM = LLVMModuleSet::getLLVMModuleSet()->getModule(0);
}

void BTAnalyzer::run()
{
    initialize();
    analyze();
}

void BTAnalyzer::initialize()
{
    extractAPI();
    extractHCI();
    buildSummaryInfo();
}

void BTAnalyzer::buildSummaryInfo()
{
    SVFIRBuilder builder;
    pag = builder.build(svfM);
    icfg = pag->getICFG();
    // AndersenWaveDiff* ander = AndersenWaveDiff::createAndersenWaveDiff(pag);
    // callgraph = ander->getPTACallGraph();   
    // SVFGBuilder memSSA;
    // svfg = memSSA.buildFullSVFG(ander);
}

void BTAnalyzer::printGlobals()
{
    SVFModule* mod = pag->getModule();
    std::cout << "======================= Global Variables =======================" << endl;

    for(auto it=mod->global_begin(),eit=mod->global_end();it!=eit;++it)
    {
        GlobalVariable* gvar = *it;
        
        Type* ty = gvar->getType()->getPointerElementType();
        if(ty->isStructTy()){
            std::string structName = ty->getStructName().str();
            std::string Name = gvar->getName().str();
            std::string Loc = SVFUtil::getSourceLoc(gvar);
            cout<< Name << "\t" << structName << "\t" << Loc << endl;
        }
    }
}

void BTAnalyzer::printFunctions()
{
    SVFModule* mod = pag->getModule();
    std::cout << "========================= Functions =========================" << endl;  

    for(auto it=mod->llvmFunBegin(),eit=mod->llvmFunEnd();it!=eit;++it)
    {
        Function* F = *it;
        std::string Name = F->getName().str();
        std::string Loc = SVFUtil::getSourceLoc(F);
        bool def = !F->isDeclaration();
        cout << Name << "\t" << def << "\t" << Loc << endl;
    }
}
