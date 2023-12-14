#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define DEVICE_FILE "/dev/hcsr04" // 드라이버에 의해 생성된 장치 파일

int main() {
    int fd;
    char distance[10];

    // 장치 파일 열기
    fd = open(DEVICE_FILE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open the device");
        return errno;
    }

    // 거리 읽기
    if (read(fd, distance, sizeof(distance)) < 0) {
        perror("Failed to read the distance");
        return errno;
    }

    printf("Distance: %s cm\n", distance);

    // 장치 파일 닫기
    close(fd);

    return 0;
}
