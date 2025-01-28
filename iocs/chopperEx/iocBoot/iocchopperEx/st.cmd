# ../../bin/${EPICS_HOST_ARCH}/chopperEx st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocchopperExWin64.dbd")
iocchopperExWin64_registerRecordDeviceDriver(pdbbase)

< settings.iocsh

NPChopperConfig("asyn_port", "dev_port")

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date
