#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<fcntl.h>
#include<termios.h>

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
		ch = 1;
	return ch;
}

int main(int argc, char** argv) {

	char key;

	int dev0 = open("/dev/my_button", O_RDONLY); // read only
	int dev1 = open("/dev/hcsr04", O_RDONLY);

	if (dev0 < 0) {
		printf("gpio_driver Opening was not possible!\n");
		return -1;
	}

	if (dev1 < 0) {
		printf("gpio_driver Opening was not possible!\n");
		return -1;
	}

	printf("device opening was sucessful!\n");

	char buffer[2];
	char distance_buffer[20];
	ssize_t bytes_read;

	init_keyboard();

	char prev_buffer[2] = { 0 }; // 이전 버튼 상태를 저장할 배열 선언
	int state1 = 0;//button up toggle state
	int state2 = 0;//button down toggle state

	while (1) {
		printf("program start!\n");
		key = get_key();

		if(key=='q'){
			printf("exit this program.\n");
            		break;
		}
		else{
			bytes_read = read(dev0, buffer, sizeof(buffer)); // 버튼 상태 읽기
			if (buffer[0] != prev_buffer[0] && buffer[0] == '1') { // 버튼 1이 눌렸는지 확인
            		state1 = !state1; // 측정 시작
       			}
			prev_buffer[0] = buffer[0]; // 현재 버튼 상태를 이전 상태로 저장
		
			if (state1 == 1) {
				printf("Distance: ");
				fflush(stdout); // 표준 출력 버퍼 비우기

				memset(distance_buffer, 0, sizeof(distance_buffer)); // 버퍼 초기화
				read(dev1, distance_buffer, sizeof(distance_buffer)); // 새로운 측정값 읽기
				printf("%s\n", distance_buffer); // 읽은 데이터 출력
			}
		}
		
	}

	close_keyboard();
	close(dev0);
	close(dev1);
	return 0;


}
