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

map<string, Module*> modules;
extern map<string, _DominatorTree*> DT;


void buildDTs()
{
    for(auto iter = modules.begin(),e=modules.end();
        iter!=e;iter++)
    {
        string m_name = iter->first;
        auto F = iter->second->getFunctionList().begin();
        auto Fe = iter->second->getFunctionList().end();
        for(;F!=Fe;F++)
        {
            if(F->isDeclaration())
                continue;
            DominatorTreeWrapper dt(*F);
            string f_name = F->getName().str();
            llvm::outs() << m_name + ":" + f_name + " ";
            dt.print(llvm::outs());
        }

    }

}

int main(int argc, char ** argv)
{

    int arg_num = 0;
    char **arg_value = new char*[argc];
    std::vector<std::string> moduleNameVec;
    moduleNameVec.push_back("example.bc");

    MyDomTreeNodeBase<BasicBlock> tmp(0,0);
    tmp.init();

    SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    auto F = svfModule->llvmFunBegin();
    auto e = svfModule->llvmFunEnd();
    Module* mod = (*F)->getParent();
    modules[mod->getName().str()] = mod;

    buildDTs();

    ofstream ofile("DT");
    boost::archive::binary_oarchive oa(ofile, boost::archive::no_header);
    oa << DT;
    DT.clear();
    ofile.close();

    ifstream ifile("DT");
    boost::archive::binary_iarchive ia(ifile, boost::archive::no_header);
    ia >> DT;
    buildDTs();


    //dt.print();
    return 0;

}
