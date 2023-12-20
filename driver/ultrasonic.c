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
    int dev;
    int distance;
    int led=0;
    int led_is_on=0;

    // 디바이스 파일 열기
    fd = open(DEVICE_PATH, O_RDONLY);
    dev = open("/dev/my_led",O_WRONLY);
	
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

	 if (distance < 10 && !is_led_on) {
                led = 1;
                write(dev, &led, sizeof(led));
                is_led_on = 1; // LED가 켜졌음을 표시
            } else if (distance >= 10 && is_led_on) {
                led = 0;
                write(dev, &led, sizeof(led));
                is_led_on = 0; // LED가 꺼졌음을 표시
            }	
    	}
 }
	
    // 디바이스 파일 닫기
    close_keyboard();
    close(fd);
    return 0;
}

