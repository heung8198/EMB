obj-m += step_motor_driver.o u_sonic_driver.o button_driver.o led_driver.o
KDIR = ~/working/Raspbian/kernel

RESULT = step_motor
RESULT1 = ultrasonic
RESULT2 = execute
RESULT3 = led

SRC = $(RESULT).c
SRC1 = $(RESULT1).c
SRC2 = $(RESULT2).c
SRC3 = $(RESULT3).c

CCC = arm-linux-gnueabihf-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT) $(SRC)
	$(CCC) -o $(RESULT1) $(SRC1)
	$(CCC) -o $(RESULT2) $(SRC2)
	$(CCC) -o $(RESULT3) $(SRC3)
	

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf $(RESULT)
	rm -rf $(RESULT1)
	rm -rf $(RESULT2)
	rm -rf $(RESULT3)

