#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int led_value;
    int fd;

    // 장치 파일 열기
    fd = open("/dev/my_led", O_WRONLY);
    if (fd < 0) {
        printf("Failed to open the device...");
        return -1;
    }

    printf("Enter 1 to turn on the LED and 0 to turn off the LED:\n");
    while (scanf("%d", &led_value) == 1) {
        // 유저로부터 받은 값(0 또는 1)을 장치 파일에 쓰기
        if (write(fd, &led_value, sizeof(led_value)) < 0) {
            perror("Failed to write to the device...");
            break;
        }

        printf("LED is %s.\n", led_value ? "on" : "off");
        printf("Enter 1 to turn on the LED and 0 to turn off the LED:\n");
    }

    // 장치 파일 닫기
    close(fd);
    return 0;
}

