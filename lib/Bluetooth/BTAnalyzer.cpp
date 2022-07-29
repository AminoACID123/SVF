#include "Bluetooth/BTAnalyzer.h"
#include "SVF-FE/SVFIRBuilder.h"
#include "Util/Options.h"
#include "Util/SVFUtil.h"
#include "Util/Options.h"
#include "WPA/Andersen.h"
#include <iostream>

using namespace SVF;

BTAnalyzer::BTAnalyzer(SVFModule* svfModule)
{
    // Initialize io_read/io_recv functions
    Module* m = LLVMModuleSet::getLLVMModuleSet()->getModule(0);
    assert(m->getFunction("hci_send_cmd") != nullptr);
    assert(m->getFunction("hci_send_sco") != nullptr);
    assert(m->getFunction("hci_send_acl") != nullptr);
    assert(m->getFunction("hci_req_add") != nullptr);

    io_send.push_back(m->getFunction("hci_send_cmd"));
    io_send.push_back(m->getFunction("hci_send_sco"));
    io_send.push_back(m->getFunction("hci_send_acl"));
    io_send.push_back(m->getFunction("hci_req_add"));


    // Build ICFG, PAG, CallGraph
    SVFIRBuilder builder;
    pag = builder.build(svfModule);
    icfg = pag->getICFG();
    AndersenWaveDiff* ander = AndersenWaveDiff::createAndersenWaveDiff(pag);
    callgraph = ander->getPTACallGraph();    
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

void BTAnalyzer::extractInterface()
{
    SVFModule* mod = pag->getModule();
    cout << llvm::Value::ConstantStructVal<<endl; 
    for(auto it=mod->global_begin(),eit=mod->global_end();it!=eit;++it)
    {
        GlobalVariable* gvar = *it;
        Type* ty = gvar->getType()->getPointerElementType();
        if(ty->isStructTy() && gvar->hasInitializer()){
            std::string name = ty->getStructName().str();
            if(name.find("struct.proto_ops") != name.npos){
                ConstantStruct* cs = SVFUtil::dyn_cast<ConstantStruct>(gvar->getInitializer());
                if(!cs)
                    continue;
                int n = cs->getNumOperands();
                for(int i=0;i<n;i++){
                    Constant* field = cs->getOperand(i);
                    if(Function* f = SVFUtil::dyn_cast<Function>(field)){
                        std::string name = f->getName().str();
                        if(name.find("sock_no") == name.npos)
                            interfaces.push_back(f);
                    }
                }
            }
        }
    }
    for(const Function* F:interfaces){
        llvm::outs() << F->getName() << "\n";
    }
}

void BTAnalyzer::analyze()
{
    if(interfaces.empty())
        extractInterface();

    for(Function* F : interfaces)
    {
        llvm::outs() << "Interface: " << F->getName() << "\n";
        for(Function* io : io_send)
        {
            SVFModule* m = pag->getModule();
            if(callgraph->isReachableBetweenFunctions(m->getSVFFunction(F), m->getSVFFunction(io)))
            {
                llvm::outs() << "    " << io->getName() << "\n";
            }   
        }
    }
}