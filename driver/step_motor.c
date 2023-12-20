#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define FORWARD 1
#define BACKWARD 2

int main(int argc, char **argv) {
    int file_desc, ret;
    int mode;

    // 디바이스 파일 열기
    file_desc = open("/dev/my_stepper", O_RDWR);
    if (file_desc < 0) {
        printf("Error: Could not open the device file\n");
        return -1;
    }

    // 인자가 충분한지 확인
    if (argc != 2) {
        printf("Usage: %s <mode>\n", argv[0]);
        printf("mode: 1 for forward, 2 for backward\n");
        close(file_desc);
        return -1;
    }

    // 인자로부터 모드 추출
    mode = atoi(argv[1]);

    if (mode != FORWARD && mode != BACKWARD) {
        printf("Invalid mode. Please enter 1 or 2.\n");
        close(file_desc);
        return -1;
    }

    // 커널 모듈에 모드 전송
    ret = write(file_desc, &mode, sizeof(mode));
    if (ret < 0) {
        printf("Error: Could not write to the device file\n");
        close(file_desc);
        return -1;
    }

    printf("Stepper motor control command sent successfully\n");

    // 디바이스 파일 닫기
    close(file_desc);
    return 0;
}

