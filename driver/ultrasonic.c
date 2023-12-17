#include <stdio.h>      // printf, perror, getchar 등의 함수를 위한 헤더
#include <stdlib.h>     // 일반 유틸리티 함수를 위한 헤더
#include <fcntl.h>      // open 함수를 위한 헤더
#include <unistd.h>     // read, close 함수와 관련 상수를 위한 헤더
#include <string.h>     // memset 함수를 위한 헤더

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
        printf("Distance: ");
        fflush(stdout); // 표준 출력 버퍼 비우기

        // Enter 키를 누르면 새로운 측정 수행
        if (getchar() == '\n') {
            memset(buffer, 0, sizeof(buffer)); // 버퍼 초기화
            read(fd, buffer, sizeof(buffer)); // 새로운 측정값 읽기
            printf("%s\n", buffer); // 읽은 데이터 출력
        }
		else if (getchar() == 'q') {
            break; // 종료
        }
    }
	


    // 디바이스 파일 닫기
    close(fd);
    return 0;
}

