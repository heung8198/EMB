obj-m += step_motor_driver.o serbo_motor_driver.o
KDIR = ~/working/Raspbian/kernel

RESULT1 = serbo_motor
RESULT2 = step_motor

SRC1 = $(RESULT1).c
SRC2 = $(RESULT2).c


CCC = arm-linux-gnueabihf-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT2) $(SRC2)
	$(CCC) -o $(RESULT1) $(SRC1)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf $(RESULT2)
	rm -rf $(RESULT1)
