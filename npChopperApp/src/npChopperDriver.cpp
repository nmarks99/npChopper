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

std::optional<int> NPChopper::parseReply() const {
    std::string in_string = in_buff_;
    if (in_string.length() < RETURN_CMD_LENGTH) {
        return std::nullopt;
    }

    in_string.erase(0, RETURN_CMD_LENGTH);
    int value = 0;
    try {
        value = std::stoi(in_string);
        return value;
    } catch (const std::exception &e) {
        printf("%s\n", e.what());
        return std::nullopt;
    }
}

std::optional<double> NPChopper::parseReplyDouble() const {
    std::string in_string = in_buff_;
    if (in_string.length() < RETURN_CMD_LENGTH) {
        return std::nullopt;
    }

    in_string.erase(0, RETURN_CMD_LENGTH);
    double value = 0.0;
    try {
        value = std::stof(in_string);
        return value;
    } catch (const std::exception &e) {
        printf("%s\n", e.what());
        return std::nullopt;
    }
}

static void poll_thread_C(void *pPvt) {
    NPChopper *pNPChopper = (NPChopper *)pPvt;
    pNPChopper->poll();
}

NPChopper::NPChopper(const char *asyn_port, int poll_period_ms)
    : asynPortDriver(asyn_port, MAX_CONTROLLERS,
                     asynInt32Mask | asynFloat64Mask | asynDrvUserMask | asynOctetMask | asynInt32ArrayMask,
                     asynInt32Mask | asynFloat64Mask | asynOctetMask | asynInt32ArrayMask,
                     ASYN_MULTIDEVICE | ASYN_CANBLOCK,
                     1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=1, autoConnect=1 */
                     0, 0)
    {

    poll_period_ = poll_period_ms / 1000.0;

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
    createParam(CONNECTED_STRING, asynParamInt32, &connectedIndex_);
    createParam(FREQ_SYNC_MEASURE_STRING, asynParamInt32, &freqSyncMeasureIndex_);
    createParam(FREQ_OUTER_MEASURE_STRING, asynParamInt32, &freqOuterMeasureIndex_);
    createParam(FREQ_OUT1_MEASURE_STRING, asynParamInt32, &freqOut1MeasureIndex_);
    createParam(FREQ_OUT2_MEASURE_STRING, asynParamInt32, &freqOut2MeasureIndex_);
    createParam(HARMONIC_STRING, asynParamInt32, &harmonicIndex_);
    createParam(SUB_HARMONIC_STRING, asynParamInt32, &subHarmonicIndex_);
    createParam(FREQUENCY_STRING, asynParamInt32, &frequencyIndex_);
    createParam(WHEEL_STRING, asynParamInt32, &wheelIndex_);
    createParam(SYNC_SOURCE_STRING, asynParamInt32, &syncSourceIndex_);
    createParam(MODE_STRING, asynParamInt32, &modeIndex_);
    createParam(PHASE_DELAY_STRING, asynParamFloat64, &phaseDelayIndex_);

    // Get device info
    writeReadController("IDN?");
    printf("Device Found: %s\n", in_buff_);

    epicsThreadCreate("NPChopperPoller", epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)poll_thread_C, this);
}

void NPChopper::poll() {
    std::optional<int> retval;
    std::optional<double> retval_double;
    bool comm_ok = true;
    while (true) {
        lock();

        comm_ok = true;

        comm_ok = writeReadController("FR1?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(freqSyncMeasureIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("FR2?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(freqOuterMeasureIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("FR3?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(freqOut1MeasureIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("FR4?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(freqOut2MeasureIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("HAR?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(harmonicIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("SUB?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(subHarmonicIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("OSC?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(frequencyIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("WHL?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(wheelIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        comm_ok = writeReadController("SYN?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(syncSourceIndex_, retval.value());
        } else {
            comm_ok = false;
        }
        
        comm_ok = writeReadController("MOD?");
        retval = parseReply();
        if (retval.has_value() && comm_ok) {
            setIntegerParam(modeIndex_, retval.value());
        } else {
            comm_ok = false;
        }

        // only phase delay query replies with a floating point number
        comm_ok = writeReadController("PHS?");
        retval_double = parseReplyDouble();
        if (retval_double.has_value() && comm_ok) {
            setDoubleParam(phaseDelayIndex_, retval_double.value());
        } else {
            comm_ok = false;
        }

        if (comm_ok) {
            setIntegerParam(connectedIndex_, 1);
        } else {
            setIntegerParam(connectedIndex_, 0);
        }


        callParamCallbacks();
        unlock();
        epicsThreadSleep(poll_period_);
    }
}

asynStatus NPChopper::writeInt32(asynUser *pasynUser, epicsInt32 value) {
    int function = pasynUser->reason;
    bool comm_ok = true;

    if (function == harmonicIndex_) {
        snprintf(out_buff_, IO_BUFFER_SIZE, "HAR%d", value);
        comm_ok = writeController();
    } else if (function == subHarmonicIndex_) {
        snprintf(out_buff_, IO_BUFFER_SIZE, "SUB%d", value);
        comm_ok = writeController();
    } else if (function == frequencyIndex_) {
        snprintf(out_buff_, IO_BUFFER_SIZE, "OSC%d", value*100);
        comm_ok = writeController();
    } else if (function == wheelIndex_) {
        snprintf(out_buff_, IO_BUFFER_SIZE, "WHL%d", value);
        comm_ok = writeController();
    } else if (function == syncSourceIndex_) {
        snprintf(out_buff_, IO_BUFFER_SIZE, "SYN%d", value);
        comm_ok = writeController();
    } else if (function == modeIndex_) {
        snprintf(out_buff_, IO_BUFFER_SIZE, "MOD%d", value);
        comm_ok = writeController();
    }

    callParamCallbacks();
    return comm_ok ? asynSuccess : asynError;
}

asynStatus NPChopper::writeFloat64(asynUser *pasynUser, epicsFloat64 value) {
    int function = pasynUser->reason;
    bool comm_ok = true;

    if (function == phaseDelayIndex_) {
        snprintf(out_buff_, IO_BUFFER_SIZE, "PHS%d", static_cast<int>(value*10.0));
        comm_ok = writeController();
    }

    callParamCallbacks();
    return comm_ok ? asynSuccess : asynError;
}

// register function for iocsh
extern "C" int NPChopperConfig(const char *asyn_port_name, int poll_period_ms) {
    NPChopper *pNPChopper = new NPChopper(asyn_port_name, poll_period_ms);
    pNPChopper = NULL;
    return (asynSuccess);
}

static const iocshArg NPChopperArg0 = {"asyn port name", iocshArgString};
static const iocshArg NPChopperArg1 = {"Poll period (ms)", iocshArgInt};
static const iocshArg *const NPChopperArgs[2] = {&NPChopperArg0, &NPChopperArg1};
static const iocshFuncDef NPChopperFuncDef = {"NPChopperConfig", 2, NPChopperArgs};

static void NPChopperCallFunc(const iocshArgBuf *args) { NPChopperConfig(args[0].sval, args[1].ival); }

void NPChopperRegister(void) { iocshRegister(&NPChopperFuncDef, NPChopperCallFunc); }

extern "C" {
epicsExportRegistrar(NPChopperRegister);
}
