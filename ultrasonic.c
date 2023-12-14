#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/hcsr04" // 사용할 디바이스 파일 경로

int main() {
    int fd;
    char buffer[20];

    // 디바이스 파일 열기
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open the device file");
        return 1;
    }

    // 사용자로부터 거리 측정 명령 받기
    printf("Press Enter to measure distance, q to quit...\n");
    while (1) {
        char c = getchar();
        if (c == 'q' || c == 'Q') {
            break; // 종료
        }
        
        // 디바이스 파일에서 거리 읽기
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("Failed to read from the device file");
            break;
        }

        // 읽은 데이터 출력
        printf("Distance: %s", buffer);
    }

    // 디바이스 파일 닫기
    close(fd);

    return 0;
}
