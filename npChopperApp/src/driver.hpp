#ifndef _DRIVER_HPP_
#define _DRIVER_HPP_

#include <asynPortDriver.h>

#include <chrono>

constexpr int MAX_CONTROLLERS = 1;
constexpr double DEFAULT_POLL_TIME = 1.0;  // seconds

class NPChopper : public asynPortDriver {
   public:
    NPChopper(const char *asyn_port, const char *usb_port);
    virtual void poll(void);
    // virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    // virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars,size_t *nActual);

   private:
    double poll_time_;
};

#endif
