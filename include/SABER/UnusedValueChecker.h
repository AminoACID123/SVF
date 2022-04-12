//===- UnusedValueChecker.h -- Detecting Unused Values--------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2017>  <Anzi Xu>
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
 * LeakChecker.h
 *
 *  Created on: Apr 1, 2014
 *      Author: rockysui
 */

#ifndef UNUSEDVALUECHECKER_H_
#define UNUSEDVALUECHECKER_H_

#include "SABER/SrcSnkDDA.h"
#include "SABER/SaberCheckerAPI.h"

namespace SVF
{

/*!
 * Static Unused Value Checker
 */
class UnusedValueChecker : public SrcSnkDDA
{

public:
    /// Constructor
    UnusedValueChecker()
    {
    }
    /// Destructor
    virtual ~UnusedValueChecker()
    {
    }

    /// Analyze module
    bool runOnModule(SVFModule* module);

    /// Initialize analysis
    void initialize(SVFModule* module) override;

    /// Finalize analysis
    void finalize() override;


    /// Initialize sources and sinks
    /// For each function
    void initSrcs() override;
    void initSnks() override;
    typedef Map<const SVFGNode*,const Value*> SVFGNodeToVariableMap;
    typedef FIFOWorkList<const CallBlockNode*> CSWorkList;
    typedef ProgSlice::VFWorkList WorkList;
    typedef NodeBS SVFGNodeBS;
    typedef set<llvm::Function*> FunctionList;
    enum UNUSED_TYPE
    {
        NEVER_USED,
        CONSECUTIVE_ASSIGN,
        INIT_REASSIGN
    };
protected:
    /// Report leaks
    //@{
    virtual void reportBug(ProgSlice* slice) override;
    void reportNeverUse(const SVFGNode* src);
    // void reportPartialLeak(const SVFGNode* src);
    //@}

    /// Validate test cases for regression test purpose
    //void testsValidation(const ProgSlice* slice);
    //void validateSuccessTests(const SVFGNode* source, const SVFFunction* fun);
    //void validateExpectedFailureTests(const SVFGNode* source, const SVFFunction* fun);

    /// Record a source to its callsite
    //@{
    inline void addSrcToVariableMap(const SVFGNode* src, const Value* var)
    {
        srcToVariableMap[src] = var;
    }
    inline const Value* getSrcVariable(const SVFGNode* src)
    {
        SVFGNodeToVariableMap::iterator it =srcToVariableMap.find(src);
        assert(it!=srcToVariableMap.end() );
        return it->second;
    }
    //@}
private:
    SVFGNodeToVariableMap srcToVariableMap;
    SVFModule* module;
    bool initialized = false;
};

} // End namespace SVF

#endif /* LEAKCHECKER_H_ */
