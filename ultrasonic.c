#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    char buffer[10];
    int dev = open("/dev/ultrasonic_sensor", O_RDWR);

    if (dev == -1) {
        perror("Error opening device file");
        return -1;
    }

    while (1) {
        // 읽기 연산을 통해 초음파 센서의 거리 값을 읽어옴
        if (read(dev, buffer, sizeof(buffer)) < 0) {
            perror("Error reading from device");
            break;
        }

        // 읽어온 거리 값을 출력
        printf("Distance: %s", buffer);

        // 1초 대기
        sleep(1);
    }

    close(dev);
    return 0;
}
