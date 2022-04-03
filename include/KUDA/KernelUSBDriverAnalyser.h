#ifndef KERNEL_USB_DRIVER_ANALYSER_H_
#define KERNEL_USB_DRIVER_ANALYSER_H_
#include "Util/SVFModule.h"



class KernelUSBDriverAnalyser
{
public:
    KernelUSBDriverAnalyser(SVF::SVFModule* mod):svfModule(mod)
    {

    }
    void printDrivers();
private:
    SVF::SVFModule* svfModule;
};


#endif /* KERNEL_USB_DRIVER_ANALYSER_H_ */
