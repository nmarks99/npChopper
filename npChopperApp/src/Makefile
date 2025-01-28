# NOTE: paths relative to where they get installed (e.g. ./O.rhel9-x86_64)

TOP=../..

include $(TOP)/configure/CONFIG

# Define IOC library name
LIBRARY_IOC += npChopper

# install npChopperSupport.dbd into <top>/dbd
DBD += npChopperSupport.dbd
npChopperSupport_DBD += npChopper.dbd

# source files to be compiled and added to the library
npChopper_SRCS += driver.cpp

# Libraries needed for npChopper
npChopper_LIBS += asyn
npChopper_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
