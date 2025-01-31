#pragma once
#include <asynPortDriver.h>

#include <chrono>
#include <string>

#include "NpChopperLib.h"

constexpr int MAX_CONTROLLERS = 1;
constexpr double DEFAULT_POLL_TIME = 1.0;  // seconds
constexpr int IO_BUFFER_SIZE = 256;

static constexpr char FREQ_SYNC_STRING[] = "FREQ_SYNC";
static constexpr char FREQ_OUTER_STRING[] = "FREQ_OUTER";
static constexpr char FREQ_OUT1_STRING[] = "FREQ_OUT1";
static constexpr char FREQ_OUT2_STRING[] = "FREQ_OUT2";

class NPChopper : public asynPortDriver {
   public:
    NPChopper(const char *asyn_port, const char *usb_port);
    virtual void poll(void);
    // virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    // virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars,size_t *nActual);

   private:
    double poll_time_;
    char in_buff_[IO_BUFFER_SIZE];
    char out_buff_[IO_BUFFER_SIZE];
    char device_key_[IO_BUFFER_SIZE];

    void writeReadController(char *msg) {
        snprintf(out_buff_, IO_BUFFER_SIZE, msg);
        HidWrite(device_key_, out_buff_);
        HidRead(device_key_, in_buff_);
    }

   protected:
    int identityIndex_;
};