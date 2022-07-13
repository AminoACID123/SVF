//===- WorkList.h -- Internal worklist used in SVF---------------------------//
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
//===----------------------------------------------------------------------===//

/*
 * @file: WorkList.h
 * @author: yesen
 * @date: 06/12/2013
 * @version: 1.0
 *
 * @section LICENSE
 *
 * @section DESCRIPTION
 *
 */

#ifndef ValueMap_H_
#define ValueMap_H_
#include "BasicTypes.h"
#include "SVFUtil.h"
#include <llvm/IR/InstIterator.h>

using namespace SVF;
using namespace std;

namespace IS_VFA {

class IRManager {
public:
  typedef std::set<unsigned> NumberSetTy;
  typedef std::map<const Function*, const Function*> FunMapTy;
  typedef std::map<const Instruction*, const Instruction*> InstMapTy;
  typedef std::map<const Function *, InstMapTy> FunToInstMapTy;

private:
  FunMapTy modifiedFuns;
  NumberSetTy addedLines;
  NumberSetTy deletedLines;
  FunToInstMapTy funToInstMap;

public:
  IRManager() = default;
  IRManager(Module *o, Module *n) : oldModule(o), newModule(n) {}
  void getInstMap();
  void dump();


private:
  void processFunction(const Function *o, const Function *n);

  bool compare(const Instruction &inst1, const Instruction &inst2);

  inline int getLine(const Instruction &inst) {
    /// ln: XX fl: YY
    string loc = SVFUtil::getSourceLoc(&inst);
    size_t s = loc.find("ln");
    size_t e = loc.find("fl");

    if (s == loc.npos) {
      return -1;
    }

    const char *line = loc.substr(s, (e - 1) - (s + 4)).c_str();
    int lineno;
    sscanf(line, "%d", &lineno);
    return lineno;
  }

  inline bool isDeleted(const Instruction &inst) 
  {
    int line = getLine(inst);
    return line == -1 || deletedLines.find(line) != deletedLines.end();
  }
  inline bool isAdded(const Instruction &inst) 
  {
    int line = getLine(inst);
    return line == -1 || addedLines.find(line) != addedLines.end();
  }
  inline bool isDbgCall(const Instruction& inst)
  {
    const CallInst& call = dyn_cast<CallInst>(inst);
    return call.getCalledFunction()->getName() == "llvm.dbg.declare";
  }

private:
  const Module *oldModule;
  const Module *newModule;
};

} // End namespace IS_VFA

#endif /* VALUEMAP_H_ */
