#include "driver.hpp"

#include <epicsExport.h>
#include <epicsThread.h>
#include <iocsh.h>

#include <chrono>
#include <iostream>

#include "NpChopperLib.h"

static void poll_thread_C(void *pPvt) {
    NPChopper *pNPChopper = (NPChopper *)pPvt;
    pNPChopper->poll();
}

NPChopper::NPChopper(const char *asyn_port, const char *usb_port)
    : asynPortDriver(asyn_port, MAX_CONTROLLERS,
                     asynInt32Mask | asynFloat64Mask | asynDrvUserMask | asynOctetMask | asynInt32ArrayMask,
                     asynInt32Mask | asynFloat64Mask | asynOctetMask | asynInt32ArrayMask,
                     ASYN_MULTIDEVICE | ASYN_CANBLOCK,
                     1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=1, autoConnect=1 */
                     0, 0),
      poll_time_(DEFAULT_POLL_TIME) {
    // Connect to device

    // Discover the device
    HidDiscover();
    if (HidGetDeviceCount() < 1) {
        // printf("Chopper not found\n");
        throw std::runtime_error("Chopper not found\n");
    }

    epicsThreadCreate("NPChopperPoller", epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)poll_thread_C, this);
}

void NPChopper::poll() {
    while (true) {
        lock();

        printf("Hello\n");

        callParamCallbacks();
        unlock();
        epicsThreadSleep(poll_time_);
    }
}

// register function for iocsh
extern "C" int NPChopperConfig(const char *asyn_port_name, const char *robot_ip) {
    NPChopper *pNPChopper = new NPChopper(asyn_port_name, robot_ip);
    pNPChopper = NULL;
    return (asynSuccess);
}

static const iocshArg NPChopperArg0 = {"Asyn port name", iocshArgString};
static const iocshArg NPChopperArg1 = {"USB port name", iocshArgString};
static const iocshArg *const NPChopperArgs[2] = {&NPChopperArg0, &NPChopperArg1};
static const iocshFuncDef NPChopperFuncDef = {"NPChopperConfig", 2, NPChopperArgs};

static void NPChopperCallFunc(const iocshArgBuf *args) { NPChopperConfig(args[0].sval, args[1].sval); }

void NPChopperRegister(void) { iocshRegister(&NPChopperFuncDef, NPChopperCallFunc); }

extern "C" {
epicsExportRegistrar(NPChopperRegister);
}
