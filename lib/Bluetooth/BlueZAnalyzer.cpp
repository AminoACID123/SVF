#include "Bluetooth/BlueZAnalyzer.h"
#include "SVF-FE/SVFIRBuilder.h"
#include "Util/Options.h"
#include "Util/SVFUtil.h"
#include "Util/Options.h"
#include "WPA/Andersen.h"
#include <iostream>
using namespace SVF;

void BlueZAnalyzer::extractAPI()
{
    for(auto it=svfM->global_begin(),eit=svfM->global_end();it!=eit;++it)
    {
        GlobalVariable* gvar = *it;
        Type* ty = gvar->getType()->getPointerElementType();
        if(ty->isStructTy() && gvar->hasInitializer())
        {
            std::string name = ty->getStructName().str();
            if(name.find("struct.proto_ops") != name.npos)
            {
                ConstantStruct* cs = SVFUtil::dyn_cast<ConstantStruct>(gvar->getInitializer());
                if(!cs)
                    continue;
                int n = cs->getNumOperands();
                for(int i=0;i<n;i++)
                {
                    Constant* field = cs->getOperand(i);
                    if(Function* f = SVFUtil::dyn_cast<Function>(field))
                    {
                        std::string name = f->getName().str();
                        if(name.find("sock_no") == name.npos)
                            interfaces.push_back(f);
                    }
                }
            }
        }
    }

    llvm::outs() << "BlueZ kernel interface: \n";
    for(const Function* F:interfaces){
        llvm::outs() << F->getName() << "\n";
    }
}

void BlueZAnalyzer::extractHCI()
{
    /// Initialize hci_read/hci_recv functions
    assert(llvmM->getFunction("hci_send_cmd") != nullptr);
    assert(llvmM->getFunction("hci_send_sco") != nullptr);
    assert(llvmM->getFunction("hci_send_acl") != nullptr);
    assert(llvmM->getFunction("hci_req_add") != nullptr);
    hci_send.push_back(llvmM->getFunction("hci_send_cmd"));
    hci_send.push_back(llvmM->getFunction("hci_send_sco"));
    hci_send.push_back(llvmM->getFunction("hci_send_acl"));
    hci_send.push_back(llvmM->getFunction("hci_req_add"));

    std::list<Function *> worklist;
    std::list<int> argNumList;

    Function* F = llvmM->getFunction("hci_prepare_cmd");
    assert(F != nullptr);

    worklist.push_back(F);
    argNumList.push_back(1);
    while (!worklist.empty()) 
    {
        Function *f = worklist.front();
        worklist.pop_front();
        int arg = argNumList.front();
        argNumList.pop_front();

        for (auto U : f->users()) {
            if (auto I = SVF::SVFUtil::dyn_cast<CallInst>(U)) 
            {
                if (auto a = SVF::SVFUtil::dyn_cast<Argument>(I->getArgOperand(arg))) 
                {
                    worklist.push_back(I->getParent()->getParent());
                    argNumList.push_back(a->getArgNo());
                } else if (auto C = SVF::SVFUtil::dyn_cast<ConstantInt>(I->getOperand(arg))) 
                {
                    hci_cmds.insert(C->getZExtValue());
                }
            }
        }
    }   
}
