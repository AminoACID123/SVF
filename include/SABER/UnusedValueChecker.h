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
 * Static Memory Leak Detector
 */
class UnusedValueChecker : public SrcSnkDDA
{

public:
    typedef Map<const SVFGNode*,const CallBlockNode*> SVFGNodeToCSIDMap;
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

    /// Constructor
    UnusedValueChecker()
    {
    }
    /// Destructor
    virtual ~UnusedValueChecker()
    {
    }

    /// We start from here
    virtual bool runOnModule(SVFModule* module)
    {
        /// start analysis
        for(auto iter = module->llvmFunBegin(),e=module->llvmFunEnd();iter!=e;++iter)
        {
            functionList.insert(*iter);
        }
        
        analyze(module);
        return false;
    }

    /// Initialize sources and sinks
    //@{
    /// Initialize sources and sinks
    virtual void initSrcs() override;
    virtual void initSnks() override;
    /// Whether the function is a heap allocator/reallocator (allocate memory)
    virtual inline bool isSourceLikeFun(const SVFFunction* fun) override
    {
        return SaberCheckerAPI::getCheckerAPI()->isMemAlloc(fun);
    }
    /// Whether the function is a heap deallocator (free/release memory)
    virtual inline bool isSinkLikeFun(const SVFFunction* fun) override
    {
        return SaberCheckerAPI::getCheckerAPI()->isMemDealloc(fun);
    }
    //@}

protected:
    /// Report leaks
    //@{
    virtual void reportBug(ProgSlice* slice) override;
    void reportNeverFree(const SVFGNode* src);
    void reportPartialLeak(const SVFGNode* src);
    //@}

    /// Validate test cases for regression test purpose
    //void testsValidation(const ProgSlice* slice);
    //void validateSuccessTests(const SVFGNode* source, const SVFFunction* fun);
    //void validateExpectedFailureTests(const SVFGNode* source, const SVFFunction* fun);

    /// Record a source to its callsite
    //@{
    inline void addSrcToCSID(const SVFGNode* src, const CallBlockNode* cs)
    {
        srcToCSIDMap[src] = cs;
    }
    inline const CallBlockNode* getSrcCSID(const SVFGNode* src)
    {
        SVFGNodeToCSIDMap::iterator it =srcToCSIDMap.find(src);
        assert(it!=srcToCSIDMap.end() && "source node not at a callsite??");
        return it->second;
    }
    //@}
private:
    SVFGNodeToCSIDMap srcToCSIDMap;
    FunctionList functionList;
    SVFModule* module;
};

} // End namespace SVF

#endif /* LEAKCHECKER_H_ */
