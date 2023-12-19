#include <stdio.h>      // printf, perror, getchar 등의 함수를 위한 헤더
#include <stdlib.h>     // 일반 유틸리티 함수를 위한 헤더
#include <fcntl.h>      // open 함수를 위한 헤더
#include <unistd.h>     // read, close 함수와 관련 상수를 위한 헤더
#include <string.h>     // memset 함수를 위한 헤더

#define DEVICE_PATH "/dev/hcsr04" // 사용할 디바이스 파일 경로

int main() {
    int fd;
    int distance;

    // 디바이스 파일 열기
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open the device file");
        return 1;
    }


    // 사용자로부터 거리 측정 명령 받기
	while (1) {
        printf("Distance: ");
        fflush(stdout); // 표준 출력 버퍼 비우기
        read(fd, &distance, sizeof(distance)); // 새로운 측정값 읽기
        printf("%d cm\n", distance); // 읽은 데이터 출력
    }
	


    // 디바이스 파일 닫기
    close(fd);
    return 0;
}

