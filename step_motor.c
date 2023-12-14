#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/my_stepper" // 드라이버가 생성한 장치 파일 경로

int main() {
    int fd;
    unsigned int steps;

    // 장치 파일 열기
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return -1; // 대신 -1을 반환
    }

    printf("Enter the number of steps to rotate the motor: ");
    scanf("%u", &steps);

    // 드라이버에 스텝 수 쓰기
    if (write(fd, &steps, sizeof(steps)) < 0) {
        perror("Failed to write to the device");
        close(fd); // 에러 발생 시 파일 닫기
        return -1; // 대신 -1을 반환
    }

    printf("Motor rotated %u steps.\n", steps);

    // 장치 파일 닫기
    close(fd);

    return 0;
}

