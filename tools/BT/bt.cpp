//===- wpa.cpp -- Whole program analysis -------------------------------------//
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
 // Whole Program Pointer Analysis
 //
 // Author: Yulei Sui,
 */

#include "Bluetooth/BTAnalyzer.h"
#include "SVF-FE/LLVMUtil.h"
#include "WPA/WPAPass.h"
#include "Util/Options.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace SVF;

int main(int argc, char** argv)
{
    if(argc != 2){
        std::cout << "Usage: bt <BITCODE>\n";
        exit(1);
    }
    
    std::string file(argv[1]);
    //std::ifstream bcList(argv[1]);
    std::vector<std::string> moduleNameVec;

    //while(bcList >> file){
        moduleNameVec.push_back(file);
    //}

    SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    svfModule->buildSymbolTableInfo();

    BTAnalyzer bt(svfModule);
    bt.analyze();
    // bt.printGlobals();
    //bt.printFunctions();

    return 0;
}