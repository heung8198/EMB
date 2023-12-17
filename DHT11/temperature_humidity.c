#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main() {
	int dev;
	char buffer[5];
	ssize_t bytes_read;

	char temp, humidity;

	dev = open("/dev/DHT-11", O_RDONLY);

	if (dev < 0) {
		printf("Error: Could not open the device file\n");
		return -1;
	}

	printf("Opening was successfull!\n");

	bytes_read = read(dev, buffer, sizeof(buffer));
	if (bytes_read == -1) {
		printf("Failed read temp & humidity\n");
	}

	temp = buffer[2];
	humidity = buffer[0];

	printf("temperature is %f", temp);
	printf("humidity is %f", humidity);

}
