#include "npChopperDriver.hpp"

#include <epicsExport.h>
#include <epicsThread.h>
#include <iocsh.h>

#include <chrono>
#include <iostream>

bool NPChopper::writeReadController(const char *msg) {
    snprintf(out_buff_, IO_BUFFER_SIZE, "%s", msg);
    bool status_ok = HidQuery(device_key_, out_buff_, in_buff_);
    return status_ok;
}

bool NPChopper::writeController(const char *msg) {
    snprintf(out_buff_, IO_BUFFER_SIZE, "%s", msg);
    bool status_ok = HidWrite(device_key_, out_buff_);
    return status_ok;
}

bool NPChopper::writeController() {
    bool status_ok = HidWrite(device_key_, out_buff_);
    return status_ok;
}

std::optional<double> NPChopper::parseReply() const {
    std::string in_string = in_buff_;
    if (in_string.length() < 3) {
        return std::nullopt;
    }

    in_string.erase(0, 3);
    double value = 0.0;
    try {
        value = std::stof(in_string);
        return value;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl; // TODO: use asynPrint
        return std::nullopt;
    }

}

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

    // HidSetLogging(true); // logs messages to ChopperLib.txt
    // Connect to device, panic if not found
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
    createParam(FREQ_SYNC_STRING, asynParamFloat64, &freqSyncIndex_);
    createParam(FREQ_OUTER_STRING, asynParamFloat64, &freqOuterIndex_);
    createParam(FREQ_OUT1_STRING, asynParamFloat64, &freqOut1Index_);
    createParam(FREQ_OUT2_STRING, asynParamFloat64, &freqOut2Index_);
    createParam(HARMONIC_MULT_STRING, asynParamInt32, &harmonicMultIndex_);

    // Get device info
    writeReadController("IDN?");
    printf("Device Identity: %s\n", in_buff_);

    epicsThreadCreate("NPChopperPoller", epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)poll_thread_C, this);
}

void NPChopper::poll() {
    std::optional<double> retval;
    bool comm_ok = true;
    while (true) {
        lock();

        comm_ok = true;

        comm_ok = writeReadController("FR1?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setDoubleParam(freqSyncIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("FR2?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setDoubleParam(freqOuterIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("FR3?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setDoubleParam(freqOut1Index_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("FR4?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setDoubleParam(freqOut2Index_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("HAR?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(harmonicMultIndex_, static_cast<int>(retval.value()));
        } else {
            comm_ok = false;
        }

        if (!comm_ok) {
            // create and set asyn parameter to indicate a comm error
            printf("Communication error\n");
        }

        callParamCallbacks();
        unlock();
        epicsThreadSleep(poll_time_);
    }
}

asynStatus NPChopper::writeInt32(asynUser *pasynUser, epicsInt32 value) {
    int function = pasynUser->reason;
    asynStatus asyn_status = asynStatus::asynSuccess;

    if (function == harmonicMultIndex_) {
        printf("Setting H = %ld\n", value);
        snprintf(out_buff_, IO_BUFFER_SIZE, "HAR%d", value);
        writeController();
    }

    callParamCallbacks();
    return asyn_status;
}


asynStatus NPChopper::writeFloat64(asynUser *pasynUser, epicsFloat64 value) {
    int function = pasynUser->reason;
    asynStatus asyn_status = asynStatus::asynSuccess;

    // if (function == freqSyncOutIndex_) {
    //     printf("Setting Sync = %lf\n", value);
    // } else if ( function == freqOuterOutIndex_) {
    //     printf("Setting Outer = %lf\n", value);
    // } else if ( function == freqOut1OutIndex_) {
    //     printf("Setting Out1 = %lf\n", value);
    // } else if ( function == freqOut2OutIndex_) {
    //     printf("Setting Out2 = %lf\n", value);
    // }

    callParamCallbacks();
    return asyn_status;
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
