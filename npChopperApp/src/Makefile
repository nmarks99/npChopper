# NOTE: paths relative to where they get installed (e.g. ./O.rhel9-x86_64)

TOP=../..

include $(TOP)/configure/CONFIG

# Install vendor library
LIB_INSTALLS += ..\NpChopperLib.lib
BIN_INSTALLS += ..\NpChopperLib.dll
BIN_INSTALLS += ..\NpHIDLib.dll

# Define IOC library name
LIBRARY_IOC += npChopper

# install npChopperSupport.dbd into <top>/dbd
DBD += npChopperSupport.dbd
npChopperSupport_DBD += npChopper.dbd

# source files to be compiled and added to the library
npChopper_SRCS += npChopperDriver.cpp

# Libraries needed for npChopper
npChopper_LIBS += asyn
npChopper_LIBS += NPChopperLib
npChopper_LIBS += $(EPICS_BASE_IOC_LIBS)

# build and install npChopperTest
PROD += npChopperTerm
npChopperTerm_SRCS = npChopperTerm.cpp
npChopperTerm_LIBS = NPChopperLib

# Force C++17 so we can use things like std::optional
USR_CXXFLAGS_WIN32 += /std:c++17

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
