//===- saber.cpp -- Source-sink bug checker------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2017>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // Saber: Software Bug Check.
 //
 // Author: Yulei Sui,
 */

#include "SVF-FE/LLVMUtil.h"
#include "Util/Serialize.h"

using namespace llvm;
using namespace SVF;



int main(int argc, char ** argv)
{

    int arg_num = 0;
    char **arg_value = new char*[argc];
    std::vector<std::string> moduleNameVec;
    moduleNameVec.push_back("DTUpdate.bc");
    //SVFUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    //cl::ParseCommandLineOptions(arg_num, arg_value,
    //                            "Source-Sink Bug Detector\n");
    SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    MyDomTreeNodeBase<BasicBlock> tmp(0,0);
    llvm::outs() << tmp.getDFSNumInOff()<<"\n";
    llvm::outs() << tmp.getDFSNumOutOff()<<"\n";
    auto F = svfModule->llvmFunBegin();
    llvm::outs()<<(*F)->getName();
    PostDominatorTreeWrapper dt(**F);

    //ofstream file("/home/xaz/DT_new");
    //boost::archive::binary_oarchive oa(file, boost::archive::no_header);

    //dt.print();
    return 0;

}
