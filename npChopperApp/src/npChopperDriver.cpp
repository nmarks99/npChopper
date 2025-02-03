#include "npChopperDriver.hpp"

#include <epicsExport.h>
#include <epicsThread.h>
#include <iocsh.h>

#include <chrono>
#include <iostream>

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

    // Connect to device, panic if not found
    HidSetLogging(true);
    HidDiscover();
    if (HidGetDeviceCount() < 1) {
        throw std::runtime_error("Chopper not found\n");
    }
    int n = HidGetDeviceKeys(in_buff_);
    if (n < 0) {
        throw std::runtime_error("Chopper device key not found\n");
    }
    strncpy(device_key_, in_buff_, IO_BUFFER_SIZE);
    
    // Create asyn params
    createParam(FREQ_SYNC_IN_STRING, asynParamFloat64, &freqSyncInIndex_);
    createParam(FREQ_OUTER_IN_STRING, asynParamFloat64, &freqOuterInIndex_);
    createParam(FREQ_OUT1_IN_STRING, asynParamFloat64, &freqOut1InIndex_);
    createParam(FREQ_OUT2_IN_STRING, asynParamFloat64, &freqOut2InIndex_);
    createParam(FREQ_SYNC_OUT_STRING, asynParamFloat64, &freqSyncOutIndex_);
    createParam(FREQ_OUTER_OUT_STRING, asynParamFloat64, &freqOuterOutIndex_);
    createParam(FREQ_OUT1_OUT_STRING, asynParamFloat64, &freqOut1OutIndex_);
    createParam(FREQ_OUT2_OUT_STRING, asynParamFloat64, &freqOut2OutIndex_);

    // Get device info
    writeReadController("IDN?");
    printf("Device Identity: %s\n", in_buff_);

    epicsThreadCreate("NPChopperPoller", epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)poll_thread_C, this);
}

void NPChopper::poll() {
    double double_val = 0.0;
    while (true) {
        lock();

        writeReadController("FR1?");
        in_string_.erase(0, 3); 
        double_val = std::stof(in_string_);
        setDoubleParam(freqSyncInIndex_, double_val);

        writeReadController("FR2?");
        in_string_.erase(0, 3); 
        double_val = std::stof(in_string_);
        setDoubleParam(freqOuterInIndex_, double_val);

        writeReadController("FR3?");
        in_string_.erase(0, 3); 
        double_val = std::stof(in_string_);
        setDoubleParam(freqOut1InIndex_, double_val);

        writeReadController("FR4?");
        in_string_.erase(0, 3); 
        double_val = std::stof(in_string_);
        setDoubleParam(freqOut2InIndex_, double_val);

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
