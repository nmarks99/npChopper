#pragma once
#include <asynPortDriver.h>

#include <chrono>
#include <string>
#include <optional>

#include "NpChopperLib.h"

constexpr int MAX_CONTROLLERS = 1;
constexpr double DEFAULT_POLL_TIME = 1.0;  // seconds
constexpr int IO_BUFFER_SIZE = 256;

static constexpr char FREQ_SYNC_IN_STRING[] = "FREQ_SYNC_IN";
static constexpr char FREQ_OUTER_IN_STRING[] = "FREQ_OUTER_IN";
static constexpr char FREQ_OUT1_IN_STRING[] = "FREQ_OUT1_IN";
static constexpr char FREQ_OUT2_IN_STRING[] = "FREQ_OUT2_IN";

static constexpr char FREQ_SYNC_OUT_STRING[] = "FREQ_SYNC_OUT";
static constexpr char FREQ_OUTER_OUT_STRING[] = "FREQ_OUTER_OUT";
static constexpr char FREQ_OUT1_OUT_STRING[] = "FREQ_OUT1_OUT";
static constexpr char FREQ_OUT2_OUT_STRING[] = "FREQ_OUT2_OUT";

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
    std::string in_string_;

    // Writes the message to the controller, reads the response
    bool writeReadController(const char *msg);

    // Parses reply stored in in_string_ and attempts to convert to a double
    // TODO: maybe better to make const and do not modify in_string_
    std::optional<double> parseReply();


   protected:
    int freqSyncInIndex_;
    int freqOuterInIndex_;
    int freqOut1InIndex_;
    int freqOut2InIndex_;

    int freqSyncOutIndex_;
    int freqOuterOutIndex_;
    int freqOut1OutIndex_;
    int freqOut2OutIndex_;
};