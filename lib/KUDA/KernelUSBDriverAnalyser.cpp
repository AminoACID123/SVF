#include "KUDA/KernelUSBDriverAnalyser.h"

void KernelUSBDriverAnalyser:: printDrivers()
{
    int count = 0;
    for(auto iter=svfModule->global_begin(),e=svfModule->global_end();iter!=e;iter++)
    {
        SVF::GlobalVariable *g_var = *iter;
        if(g_var->getValueType()->getTypeID() == llvm::Type::StructTyID)
        {
            llvm::StringRef struct_name = g_var->getValueType()->getStructName();
            if(struct_name.find("struct.usb_driver") != struct_name.npos)
            {
                llvm::outs()<<struct_name<<"\n";
                count++;
            }
        }
    }

    llvm::outs()<<count<<"\n";
}