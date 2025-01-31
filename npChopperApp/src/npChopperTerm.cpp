#include "NpChopperLib.h"
#include <stdio.h>
#include <iostream>
#include <string>

constexpr int IO_BUFFER_SIZE = 256;

int main() {

	// Discover the device
	HidDiscover();
	if (HidGetDeviceCount() < 1) {
		printf("Chopper not found\n");
		return 1;
	}

	char device_key[IO_BUFFER_SIZE];
	int n = HidGetDeviceKeys(device_key);
	if (n < 0) {
		printf("Device key not found\n");
		return 1;
	}

	char out_buff[IO_BUFFER_SIZE];
	char in_buff[IO_BUFFER_SIZE];

	printf("--------------------------------------\n");
	printf("NP 3502 Chopper Communication Terminal\n");
	printf("Type 'q' to quit\n");
	printf("--------------------------------------\n\n");
	std::string user_in;
	while (true) {
		printf("> ");
		std::getline(std::cin, user_in);
		if (user_in == "q") {
			break;
		}
		sprintf(out_buff, user_in.c_str());
		HidWrite(device_key, out_buff);
		HidRead(device_key, in_buff);
		printf("%s\n\n", in_buff);
	}

	HidShutdown();

	return 0;
}
