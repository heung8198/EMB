#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    int fd;
    int led_value;

    // 인자가 충분하지 않은 경우 오류 메시지 출력
    if (argc != 2) {
        printf("Usage: %s <0|1>\n", argv[0]);
        return -1;
    }

    // 인자로 받은 값을 정수로 변환
    led_value = atoi(argv[1]);

    // 유효한 값(0 또는 1)인지 확인
    if (led_value != 0 && led_value != 1) {
        printf("Invalid input. Please enter 1 to turn on the LED or 0 to turn it off.\n");
        return -1;
    }

    // 장치 파일 열기
    fd = open("/dev/my_led", O_WRONLY);
    if (fd < 0) {
        perror("Failed to open the device /dev/my_led");
        return -1;
    }

    // 유저로부터 받은 값(0 또는 1)을 장치 파일에 쓰기
    if (write(fd, &led_value, sizeof(led_value)) < 0) {
        printf("Failed to write to the device...");
        return -1;
    } else {
        printf("LED is %s.\n", led_value ? "on" : "off");
    }

    // 장치 파일 닫기
    close(fd);
    return 0;
}
