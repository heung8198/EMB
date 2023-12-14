obj-m += step_motor_driver.o  u_sonic_driver.o 
KDIR = ~/working/Raspbian/kernel

RESULT1 = ultrasonic

SRC1 = $(RESULT1).c

CCC = arm-linux-gnueabihf-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT1) $(SRC1)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf $(RESULT1)
