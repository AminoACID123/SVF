#include "Bluetooth/BlueKitchenAnalyzer.h"
#include "Bluetooth/BlueKitchen_api.h"
#include "SVF-FE/CallGraphBuilder.h"
#include "SVF-FE/SVFIRBuilder.h"
#include "Util/Options.h"
#include "Util/SVFUtil.h"
#include "Util/Options.h"
#include "WPA/Andersen.h"
#include <iostream>

using namespace SVF;

void BlueKitchenAnalyzer::extractHCI()
{
    Function* HCI[3] = {
        llvmM->getFunction("hci_send_cmd_packet"),
        llvmM->getFunction("hci_send_acl_packet_fragments"),
        llvmM->getFunction("hci_send_sco_packet_buffer"),
    };
    // assert(HCI[0] && HCI[1] && HCI[2]);
    hci_send.push_back(HCI[0]);
    hci_send.push_back(HCI[1]);
    hci_send.push_back(HCI[2]);

    Function* F = llvmM->getFunction("hci_init");
    for (inst_iterator II = inst_begin(F), E = inst_end(F); II != E; ++II)
    {
        Instruction *inst = &*II;
        if(SVFUtil::isa<CallInst>(inst) && !SVFUtil::isIntrinsicInst(inst)){
            CallInst* call = SVFUtil::dyn_cast<CallInst>(inst);
            if(!call->getCalledFunction()){
                Function* recv = SVFUtil::dyn_cast<Function>(call->getArgOperand(0));
                hci_recv.push_back(recv);
                break;
            }
        }
    }

    // GlobalVariable* hci = llvmM->getGlobalVariable("hci_stack", true);
    // assert(hci != nullptr);

    // // %1 = load %struct.hci_stack_t*, %struct.hci_stack_t** @hci_stack, align 8
    // // %2 = %133 = getelementptr inbounds %struct.hci_stack_t, %struct.hci_stack_t* %1, i32 0, i32 0
    // // %3 = load %struct.hci_transport_t*, %struct.hci_transport_t** %2
    // // %4 = getelementptr inbounds %struct.hci_transport_t, %struct.hci_transport_t* %3, i32 0, i32 6

    // for (Module::iterator fi = llvmM->begin(), efi = llvmM->end(); fi != efi; ++fi)
    // {
    //     Function* F = &(*fi);
    //     for (inst_iterator II = inst_begin(F), E = inst_end(F); II != E; ++II)
    //     {
    //         Instruction *inst = &*II;
    //         if(GetElementPtrInst* gep = SVFUtil::dyn_cast<GetElementPtrInst>(inst))
    //         {
    //             if(gep->getNumIndices() != 2)
    //                 continue;
    //             Type* elemTy = gep->getPointerOperandType()->getPointerElementType();
    //             if(elemTy->isStructTy() && elemTy->getStructName() == "struct.hci_transport_t"){
    //                 ConstantInt* idx1 = SVFUtil::dyn_cast<ConstantInt>(gep->getOperand(1));
    //                 ConstantInt* idx2 = SVFUtil::dyn_cast<ConstantInt>(gep->getOperand(2));
    //                 if(idx1->getZExtValue() == 0 && idx2->getZExtValue() == 6)
    //                     llvm::outs() << F->getName() << "\n";
    //             }
    //         }
    //     }
    // }
}

void BlueKitchenAnalyzer::extractAPI()
{
    for(auto name : BlueKitchen_API)
    {
        Function* F = llvmM->getFunction(name);
        if(F == nullptr){
            // llvm::outs() << "API not found: " << name << "\n";
            continue;
        }
        else{
            interfaces.push_back(F);
        }
    }
}

void BlueKitchenAnalyzer::analyze()
{
    // PTACallGraph* cg = new PTACallGraph();
    // CallGraphBuilder cgbuilder = CallGraphBuilder(cg, icfg);
    // callgraph = cgbuilder.buildCallGraph(svfM);

    // Function* handler = llvmM->getFunction("hci_add_event_handler");
    // for(Function* F : interfaces)
    // {
    //     const SVFFunction* src = svfM->getSVFFunction(F);
    //     const SVFFunction* dst = svfM->getSVFFunction(handler);
    //     if(callgraph->isReachableBetweenFunctions(src, dst)){
    //         llvm::outs() << src->getName() << " : " << dst->getName() << "\n";
    //     }
    // }

    Function* recv = hci_recv.front();
    SwitchInst* dispatcher = nullptr;
    for (inst_iterator II = inst_begin(recv), E = inst_end(recv); II != E; ++II)
    {
        Instruction* inst = &*II;
        dispatcher = SVFUtil::dyn_cast<SwitchInst>(inst);
        if(dispatcher!= nullptr)
            break;
    }

    for(SwitchInst::CaseHandle c : dispatcher->cases())
    {
        int cond = c.getCaseValue()->getZExtValue();
        BasicBlock* BB = c.getCaseSuccessor();
        for (auto II = BB->begin(), E = BB->end(); II != E; ++II)
        {
            Instruction* inst = &*II;
            CallInst* call = SVFUtil::dyn_cast<CallInst>(inst);
            if(call != nullptr && !SVFUtil::isIntrinsicInst(inst)){
                Function* callee = call->getCalledFunction();
                switch (cond)
                {
                    case HCI_EVENT_PACKET: analyzeEventHandler(callee);break;
                    case HCI_ACL_DATA_PACKET: analyzeACLHandler(callee);break;
                    case HCI_SCO_DATA_PACKET: analyzeSCOHandler(callee);break;   
                    default:
                        break;
                }
            }
        }
    }
}

void BlueKitchenAnalyzer::analyzeEventHandler(Function* F)
{
    SwitchInst* dispatcher = nullptr;
    for (inst_iterator II = inst_begin(F), E = inst_end(F); II != E; ++II)
    {
        Instruction* inst = &*II;
        dispatcher = SVFUtil::dyn_cast<SwitchInst>(inst);
        if(dispatcher != nullptr)
            break;
    }    
    for(SwitchInst::CaseHandle c : dispatcher->cases())
    {
        int cond = c.getCaseValue()->getZExtValue();
    }
}