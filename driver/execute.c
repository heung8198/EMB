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
	int dev1 = open("/dev/hcrs04", O_RDONLY);
	int dev2 = open("/dev/my_led", O_WRONLY);

	if (dev0 < 0) {
		printf("gpio_driver Opening was not possible!\n");
		return -1;
	}

	if (dev1 < 0) {
		printf("gpio_driver Opening was not possible!\n");
		return -1;
	}

	if (dev2 < 0) {
		printf("gpio_driver Opening was not possible!\n");
		return -1;
	}

	printf("device opening was sucessful!\n");

	char buffer[2];
	int distance;
	int led[2];
	ssize_t bytes_read;

	//button state read
	bytes_read = read(dev0, buffer, sizeof(buffer));
	//button up state buffer[0], button down state buffer[1]

	init_keyboard();

	char prev_buffer[2] = { 0 }; // 이전 버튼 상태를 저장할 배열 선언
	int state1 = 0;//button up toggle state
	int state2 = 0;//button down toggle state

	while (1) {

		key = get_key();

		if (key == '\n') {
			printf("program start!\n");
			if (buffer[0] != prev_buffer[0]) { // 버튼 1이 눌렸는지 확인
				state1 = !state1;
			}
			
			// 현재 버튼 상태를 이전 상태로 저장
			prev_buffer[0] = buffer[0];
			

			if (state1 == 1) {
				printf("Distance: ");
				fflush(stdout); // 표준 출력 버퍼 비우기

				read(dev1, &distance, sizeof(distance)); // 새로운 측정값 읽기
				printf("%d cm\n", distance); // 읽은 데이터 출력

				if (distance < 10) {
					led[0] = 1;
					write(dev2, &led, sizeof(led));
				}
				else {
					led[0] = 0;
					write(dev2, &led, sizeof(led));
				}
			}
			
		
		}
		

	
	}

	close_keyboard();
	close(dev0);
	close(dev1);
	return 0;


}
