//===- LeakChecker.cpp -- Unused Value Checker ------------------------------//
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
 * LeakChecker.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: Yulei Sui
 */

#include "Util/Options.h"
#include "SVF-FE/LLVMUtil.h"
#include "SABER/UnusedValueChecker.h"
#include "SVF-FE/PAGBuilder.h"

using namespace SVF;
using namespace SVFUtil;



bool UnusedValueChecker::runOnModule(SVFModule* module)
{
    this->module = module;
    analyze(module);
    return false;
}


/// Initialize analysis
void UnusedValueChecker::initialize(SVFModule* module)
{
    PAGBuilder builder;
    PAG* pag = builder.build(module);

    AndersenWaveDiff* ander = AndersenWaveDiff::createAndersenWaveDiff(pag);
    svfg =  memSSA.buildFullSVFG(ander);
    setGraph(memSSA.getSVFG());
    ptaCallGraph = ander->getPTACallGraph();
    //AndersenWaveDiff::releaseAndersenWaveDiff();
    /// allocate control-flow graph branch conditions
    getPathAllocator()->allocate(getPAG()->getModule());
    
    pag->dump("pag");
    pag->getICFG()->dump("icfg");
    svfg->dump("svfg");

    initSrcs();
    initSnks();
}

/*
void UnusedValueChecker::analyze(SVFModule* module)
{

    initialize(module);

    ContextCond::setMaxCxtLen(Options::CxtLimit);

    for (SVFGNodeSetIter iter = sourcesBegin(), eiter = sourcesEnd();
            iter != eiter; ++iter)
    {
        setCurSlice(*iter);

        DBOUT(DGENERAL, outs() << "Analysing slice:" << (*iter)->getId() << ")\n");
        ContextCond cxt;
        DPIm item((*iter)->getId(),cxt);
        forwardTraverse(item);

        /// do not consider there is bug when reaching a global SVFGNode
        /// if we touch a global, then we assume the client uses this memory until the program exits.
        if (getCurSlice()->isReachGlobal())
        {
            DBOUT(DSaber, outs() << "Forward analysis reaches globals for slice:" << (*iter)->getId() << ")\n");
        }
        else
        {
            DBOUT(DSaber, outs() << "Forward process for slice:" << (*iter)->getId() << " (size = " << getCurSlice()->getForwardSliceSize() << ")\n");

            for (SVFGNodeSetIter sit = getCurSlice()->sinksBegin(), esit =
                        getCurSlice()->sinksEnd(); sit != esit; ++sit)
            {
                ContextCond cxt;
                DPIm item((*sit)->getId(),cxt);
                backwardTraverse(item);
            }

            DBOUT(DSaber, outs() << "Backward process for slice:" << (*iter)->getId() << " (size = " << getCurSlice()->getBackwardSliceSize() << ")\n");

            if(Options::DumpSlice)
                annotateSlice(_curSlice);

            if(_curSlice->AllPathReachableSolve()== true)
                _curSlice->setAllReachable();

            DBOUT(DSaber, outs() << "Guard computation for slice:" << (*iter)->getId() << ")\n");
        }

        reportBug(getCurSlice());
    }

    finalize();
}
*/

void UnusedValueChecker::finalize()
{
    
}

/*!
 * Initialize sources
 */
void UnusedValueChecker::initSrcs()
{ 
    PAG* pag = getPAG();
    SVFG* svfg = (SVFG*)getSVFG();
    MemSSA* mssa = svfg->getMSSA();
    
    //pag->getInstPAGEdgeList();
    
    for(auto fit=module->llvmFunBegin(),fe=module->llvmFunEnd();fit!=fe;++fit)
    { 
        SVFFunction func(*fit);
        SVFG::FormalINSVFGNodeSet ns = svfg->getFormalINSVFGNodes(&func);
        for(auto nit=svfg->getVFGNodeBegin(&func),neit=svfg->getVFGNodeEnd(&func);nit!=neit;++nit)
        {
            VFGNode* node = *nit;
            /*
            // Entry CHI
            if(FormalINSVFGNode::classof(node))
            {
                addToSources(node);
            }
            // Call CHI
            else if(ActualOUTSVFGNode::classof(node))
            {
                addToSources(node);
            }
            else */if(StmtSVFGNode::classof(node))
            {
                const PAGEdge* pagEdge = ((StmtSVFGNode*)node)->getPAGEdge();
                const Value* var = pagEdge->getDstNode()->getValue();
                addSrcToVariableMap(node, var);
                if(mssa->hasCHI(pagEdge))
                {
                    addToSources(node);
                }
            }
        }
    }
}


/*!
 * Initialize sinks
 */
void UnusedValueChecker::initSnks()
{
    PAG* pag = getPAG();
    SVFG* svfg = (SVFG*)getSVFG();
    MemSSA* mssa = svfg->getMSSA();
    
    //pag->getInstPAGEdgeList();
    
    for(auto fit=module->llvmFunBegin(),fe=module->llvmFunEnd();fit!=fe;++fit)
    { 
        SVFFunction func(*fit);
        SVFG::FormalINSVFGNodeSet ns = svfg->getFormalINSVFGNodes(&func);
        for(auto nit=svfg->getVFGNodeBegin(&func),neit=svfg->getVFGNodeEnd(&func);nit!=neit;++nit)
        {
            VFGNode* node = *nit;
            if(StmtSVFGNode::classof(node))
            {
                const PAGEdge* pagEdge = ((StmtSVFGNode*)node)->getPAGEdge();

                if(mssa->hasMU(pagEdge))
                {
                    addToSinks(node);
                }
            }
        }
    }
}


void UnusedValueChecker::reportNeverUse(const SVFGNode* src)
{
    const Value* var = getSrcVariable(src);
    SVFUtil::errs() << bugMsg1("\t NeverUse :") <<  " Value assigned but never used : ("
                    << getSourceLoc(var) << ")\n";
}
/*
void UnusedValueChecker::reportPartialLeak(const SVFGNode* src)
{
    const CallBlockNode* cs = getSrcCSID(src);
    SVFUtil::errs() << bugMsg2("\t PartialLeak :") <<  " memory allocation at : ("
                    << getSourceLoc(cs->getCallSite()) << ")\n";
}
*/
void UnusedValueChecker::reportBug(ProgSlice* slice)
{

    if(isAllPathReachable() == false && isSomePathReachable() == false)
    {
        reportNeverUse(slice->getSource());
    }
    /*
    else if (isAllPathReachable() == false && isSomePathReachable() == true)
    {
        reportPartialLeak(slice->getSource());
        SVFUtil::errs() << "\t\t conditional free path: \n" << slice->evalFinalCond() << "\n";
        slice->annotatePaths();
    }
    */
}


/*!
 * Validate test cases for regression test purpose
 */
/*
void UnusedValueChecker::testsValidation(const ProgSlice* slice)
{
    const SVFGNode* source = slice->getSource();
    const CallBlockNode* cs = getSrcCSID(source);
    const SVFFunction* fun = getCallee(cs->getCallSite());
    if(fun==nullptr)
        return;

    validateSuccessTests(source,fun);
    validateExpectedFailureTests(source,fun);
}


void UnusedValueChecker::validateSuccessTests(const SVFGNode* source, const SVFFunction* fun)
{

    const CallBlockNode* cs = getSrcCSID(source);

    bool success = false;

    if(fun->getName() == "SAFEMALLOC")
    {
        if(isAllPathReachable() == true && isSomePathReachable() == true)
            success = true;
    }
    else if(fun->getName() == "NFRMALLOC")
    {
        if(isAllPathReachable() == false && isSomePathReachable() == false)
            success = true;
    }
    else if(fun->getName() == "PLKMALLOC")
    {
        if(isAllPathReachable() == false && isSomePathReachable() == true)
            success = true;
    }
    else if(fun->getName() == "CLKMALLOC")
    {
        if(isAllPathReachable() == false && isSomePathReachable() == false)
            success = true;
    }
    else if(fun->getName() == "NFRLEAKFP" || fun->getName() == "PLKLEAKFP"
            || fun->getName() == "LEAKFN")
    {
        return;
    }
    else
    {
        writeWrnMsg("\t can not validate, check function not found, please put it at the right place!!");
        return;
    }

    std::string funName = source->getFun()->getName().str();

    if (success)
        outs() << sucMsg("\t SUCCESS :") << funName << " check <src id:" << source->getId()
               << ", cs id:" << *getSrcCSID(source)->getCallSite() << "> at ("
               << getSourceLoc(cs->getCallSite()) << ")\n";
    else
    {
        SVFUtil::errs() << errMsg("\t FAILURE :") << funName << " check <src id:" << source->getId()
                        << ", cs id:" << *getSrcCSID(source)->getCallSite() << "> at ("
                        << getSourceLoc(cs->getCallSite()) << ")\n";
        assert(false && "test case failed!");
    }
}

void LeakChecker::validateExpectedFailureTests(const SVFGNode* source, const SVFFunction* fun)
{

    const CallBlockNode* cs = getSrcCSID(source);

    bool expectedFailure = false;

    if(fun->getName() == "NFRLEAKFP")
    {
        if(isAllPathReachable() == false && isSomePathReachable() == false)
            expectedFailure = true;
    }
    else if(fun->getName() == "PLKLEAKFP")
    {
        if(isAllPathReachable() == false && isSomePathReachable() == true)
            expectedFailure = true;
    }
    else if(fun->getName() == "LEAKFN")
    {
        if(isAllPathReachable() == true && isSomePathReachable() == true)
            expectedFailure = true;
    }
    else if(fun->getName() == "SAFEMALLOC" || fun->getName() == "NFRMALLOC"
            || fun->getName() == "PLKMALLOC" || fun->getName() == "CLKLEAKFN")
    {
        return;
    }
    else
    {
        writeWrnMsg("\t can not validate, check function not found, please put it at the right place!!");
        return;
    }

    std::string funName = source->getFun()->getName().str();

    if (expectedFailure)
        outs() << sucMsg("\t EXPECTED-FAILURE :") << funName << " check <src id:" << source->getId()
               << ", cs id:" << *getSrcCSID(source)->getCallSite() << "> at ("
               << getSourceLoc(cs->getCallSite()) << ")\n";
    else
    {
        SVFUtil::errs() << errMsg("\t UNEXPECTED FAILURE :") << funName
                        << " check <src id:" << source->getId()
                        << ", cs id:" << *getSrcCSID(source)->getCallSite() << "> at ("
                        << getSourceLoc(cs->getCallSite()) << ")\n";
        assert(false && "test case failed!");
    }
}

*/