#pragma once
#include <asynPortDriver.h>

#include <chrono>
#include <string>
#include <optional>

#include "NpChopperLib.h"

constexpr int MAX_CONTROLLERS = 1;
constexpr double DEFAULT_POLL_PERIOD = 0.2; // seconds
constexpr int IO_BUFFER_SIZE = MAXBUFLEN;
static constexpr int RETURN_CMD_LENGTH = 3;

static constexpr char CONNECTED_STRING[] = "CONNECTED";
static constexpr char FREQ_SYNC_MEASURE_STRING[] = "FREQ_SYNC_MEASURE";
static constexpr char FREQ_OUTER_MEASURE_STRING[] = "FREQ_OUTER_MEASURE";
static constexpr char FREQ_OUT1_MEASURE_STRING[] = "FREQ_OUT1_MEASURE";
static constexpr char FREQ_OUT2_MEASURE_STRING[] = "FREQ_OUT2_MEASURE";
static constexpr char HARMONIC_STRING[] = "HARMONIC";
static constexpr char SUB_HARMONIC_STRING[] = "SUB_HARMONIC";
static constexpr char FREQUENCY_STRING[] = "FREQUENCY";
static constexpr char PHASE_DELAY_STRING[] = "PHASE_DELAY";
static constexpr char WHEEL_STRING[] = "WHEEL";
static constexpr char SYNC_SOURCE_STRING[] = "SYNC_SOURCE";
static constexpr char MODE_STRING[] = "MODE";

class NPChopper : public asynPortDriver {
   public:
    NPChopper(const char *asyn_port, int poll_period_ms);
    virtual void poll(void);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);

   private:
    double poll_period_ = DEFAULT_POLL_PERIOD;
    char in_buff_[IO_BUFFER_SIZE];
    char out_buff_[IO_BUFFER_SIZE];
    char device_key_[IO_BUFFER_SIZE];

    // Writes msg to the controller, reads the response into in_buff_
    bool writeReadController(const char *msg);

    // Writes msg to the controller
    bool writeController(const char *msg);

    // Writes out_buff_ to the controller
    bool writeController();

    // Parses reply stored in in_buff_ and attempts to convert to a int
    std::optional<int> parseReply() const;

    // Parses reply stored in in_buff_ and attempts to convert to a double
    std::optional<double> parseReplyDouble() const;


   protected:
    int connectedIndex_;
    int freqSyncMeasureIndex_;
    int freqOuterMeasureIndex_;
    int freqOut1MeasureIndex_;
    int freqOut2MeasureIndex_;
    int harmonicIndex_;
    int subHarmonicIndex_;
    int frequencyIndex_;
    int phaseDelayIndex_;
    int wheelIndex_;
    int syncSourceIndex_;
    int modeIndex_;
};