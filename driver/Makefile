obj-m += step_motor_driver.o ultrasonic_driver.o button_driver.o led_driver.o
KDIR = ~/working/Raspbian/kernel

RESULT = step_motor
RESULT1 = ultrasonic
RESULT2 = execute
RESULT3 = led
RESULT4 = button

SRC = $(RESULT).c
SRC1 = $(RESULT1).c
SRC2 = $(RESULT2).c
SRC3 = $(RESULT3).c
SRC4 = $(RESULT4).c

CCC = arm-linux-gnueabihf-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT) $(SRC)
	$(CCC) -o $(RESULT1) $(SRC1)
	$(CCC) -o $(RESULT2) $(SRC2)
	$(CCC) -o $(RESULT3) $(SRC3)
	$(CCC) -o $(RESULT4) $(SRC4)	

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf $(RESULT)
	rm -rf $(RESULT1)
	rm -rf $(RESULT2)
	rm -rf $(RESULT3)
	rm -rf $(RESULT4)
