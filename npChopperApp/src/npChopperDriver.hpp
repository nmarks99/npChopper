#pragma once
#include <asynPortDriver.h>

#include <chrono>
#include <string>
#include <optional>

#include "NpChopperLib.h"

constexpr int MAX_CONTROLLERS = 1;
constexpr double DEFAULT_POLL_TIME = 0.2;  // seconds
constexpr int IO_BUFFER_SIZE = MAXBUFLEN;

static constexpr char FREQ_SYNC_STRING[] = "FREQ_SYNC";
static constexpr char FREQ_OUTER_STRING[] = "FREQ_OUTER";
static constexpr char FREQ_OUT1_STRING[] = "FREQ_OUT1";
static constexpr char FREQ_OUT2_STRING[] = "FREQ_OUT2";
static constexpr char HARMONIC_MULT_STRING[] = "HARMONIC_MULT";

class NPChopper : public asynPortDriver {
   public:
    NPChopper(const char *asyn_port, const char *usb_port);
    virtual void poll(void);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    // virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars,size_t *nActual);

   private:
    double poll_time_;
    char in_buff_[IO_BUFFER_SIZE];
    char out_buff_[IO_BUFFER_SIZE];
    char device_key_[IO_BUFFER_SIZE];

    // Writes msg to the controller, reads the response into in_buff_
    bool writeReadController(const char *msg);

    // Writes msg to the controller
    bool writeController(const char *msg);

    // Writes out_buff_ to the controller
    bool writeController();

    // Parses reply stored in in_buff_ and attempts to convert to a double
    std::optional<double> parseReply() const;


   protected:
    int freqSyncIndex_;
    int freqOuterIndex_;
    int freqOut1Index_;
    int freqOut2Index_;
    int harmonicMultIndex_;
};