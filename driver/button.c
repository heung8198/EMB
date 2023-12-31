#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int fd;
    char button_state[2];
    ssize_t bytes_read;

    // 장치 파일 열기
    fd = open("/dev/my_button", O_RDONLY);
    if (fd < 0) {
        printf("Failed to open the device...\n");
        return -1;
    }

    printf("Reading from the button device...\n");
    while (1) {
        // 버튼 상태 읽기
        bytes_read = read(fd, &button_state, sizeof(button_state));
        if (bytes_read < 0) {
            printf("Failed to read the device...\n");
            break;
        } else if (bytes_read == 0) {
            // 데이터가 없으면 다시 시도
            continue;
        }

        // 버튼 상태 출력
        if (button_state[0] == '1') {
            printf("Button 1 (GPIO 16) state: %c\n", button_state[0]);
            break;
        }
        if (button_state[1] == '1') {
            printf("Button 2 (GPIO 20) state: %c\n", button_state[1]);
            break;
        }
    }

    // 장치 파일 닫기
    close(fd);
    return 0;
}

