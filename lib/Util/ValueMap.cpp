#include "Util/ValueMap.h"

using namespace IS_VFA;
using namespace llvm;

void IRManager::processFunction(const Function *o, const Function *n) {
  bool exit = false;
  const_inst_iterator inst1 = inst_begin(o);
  const_inst_iterator inst2 = inst_begin(n);
  const_inst_iterator e1 = inst_end(o);
  const_inst_iterator e2 = inst_end(n);

  modifiedFuns[o] = n;

  while (1) {

    while (inst1 != e1 && (isDeleted(*inst1) || isDbgCall(*inst1))) {
      ++inst1;
    }

    while (inst2 != e2 && (isAdded(*inst2) || isDbgCall(*inst2))) {
      ++inst2;
    }
    if (inst1 == e1 || inst2 == e2)
      return;

    bool match = compare(*inst1, *inst2);
    if (match)
    {
        ++inst1;
        ++inst2;
    }
    else
     return;
  }
}

bool IRManager::compare(const Instruction &inst1, const Instruction &inst2) {
  return inst1.getOpcode() == inst2.getOpcode() &&
    inst1.getNumOperands() == inst2.getNumOperands();
}
