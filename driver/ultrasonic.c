#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h> 

#define DEVICE_PATH "/dev/hcsr04" // 사용할 디바이스 파일 경로
static struct termios init_setting, new_setting;


void init_keyboard()
{
	tcgetattr(STDIN_FILENO, &init_setting);
	new_setting = init_setting;
	new_setting.c_lflag &= ~ICANON;
	new_setting.c_lflag &= ~ECHO;
	new_setting.c_cc[VMIN] = 0;
	new_setting.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_setting);
}

void close_keyboard()
{
	tcsetattr(0, TCSANOW, &init_setting);
}

char get_key()
{
	char ch = -1;

	if (read(STDIN_FILENO, &ch, 1) != 1)
		ch = -1;
	return ch;
}

int main() {
    char key;
    int fd;
    int distance;

    // 디바이스 파일 열기
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open the device file");
        return 1;
    }

	init_keyboard();
	printf("press q for quit\n");

	
    // 사용자로부터 거리 측정 명령 받기
	while (1) {
		
	key = get_key();
	if(key=='q'){
		printf("exit this program.\n");
		break;
	}
		
	else{	
        printf("Distance: ");
        fflush(stdout); // 표준 출력 버퍼 비우기
	usleep(100);
        read(fd, &distance, sizeof(distance)); // 새로운 측정값 읽기
        printf("%d cm\n", distance); // 읽은 데이터 출력
    		}
	}
	
    // 디바이스 파일 닫기
    close_keyboard();
    close(fd);
    return 0;
}

