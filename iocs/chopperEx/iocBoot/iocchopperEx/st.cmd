# ../../bin/${EPICS_HOST_ARCH}/chopperEx st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocchopperExWin64.dbd")
iocchopperExWin64_registerRecordDeviceDriver(pdbbase)

epicsEnvSet("IOCSH_PS1", "$(IOC)>")
epicsEnvSet("PREFIX", "chopperEx:")

NPChopperConfig("NpChopperAsyn", 100)
dbLoadRecords("$(NPCHOPPER)/db/npChopper.db", "P=$(PREFIX), PORT=NpChopperAsyn")

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date
