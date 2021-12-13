#ifndef UNUSEDVALUECHECKER_H_
#define UNUSEDVALUECHECKER_H_

#include "SABER/SrcSnkDDA.h"
#include "SABER/SaberCheckerAPI.h"
#include "SVF-FE/PAGBuilder.h"
namespace SVF
{

/*!
 * Static Memory Leak Detector
 */
class UnusedValueChecker
{
public:
bool runOnModule(SVFModule* module)
    {
        /// start analysis
        PAGBuilder builder;
	    PAG* pag = builder.build(module);
        pag->dump("pag");
        return false;
    }
};

} // End namespace SVF

#endif /* UNUSEDVALUECHECKER_H_ */