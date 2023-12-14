#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE "/dev/my_servo" // 드라이버가 생성한 장치 파일 경로

int main() {
    int fd;
    int pulse_width;

    // 장치 파일 열기
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }

    printf("Enter pulse width in microseconds (1000-2000) for servo motor: ");
    scanf("%d", &pulse_width);

    // 드라이버에 펄스 폭 쓰기
    char buffer[10];
    sprintf(buffer, "%d", pulse_width);
    if (write(fd, buffer, sizeof(buffer)) < 0) {
        perror("Failed to write to the device");
        close(fd);
        return -1;
    }

    printf("Set servo to pulse width: %d microseconds.\n", pulse_width);

    // 장치 파일 닫기
    close(fd);

    return 0;
}

