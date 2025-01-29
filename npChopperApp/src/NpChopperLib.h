//-----------------------------------------------------------------------
// Copyright(c) 2014 by Newport Corporation
// All Rights Reserved Worldwide
//-----------------------------------------------------------------------
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// The maximum number of devices
#define MAXDEVICES 15
// The maximum I/O transfer length
#define MAXBUFLEN 65

void HidSetLogging (bool value);
void HidSetReadTimeout (int nMilliseconds);
int HidGetDeviceCount ();
int HidGetDeviceKeys (char* DeviceKeys);
void HidDiscover ();
bool HidReadBinary (char* pDeviceKey, unsigned char* byteArr, int* nBytesRead);
bool HidRead (char* pDeviceKey, char* pBuffer);
bool HidWriteBinary (char* pDeviceKey, unsigned char* byteArr);
bool HidWrite (char* pDeviceKey, char* pBuffer);
bool HidQuery (char* pDeviceKey, char* pCommand, char* pBuffer);
void HidShutdown ();

#ifdef __cplusplus
}
#endif
