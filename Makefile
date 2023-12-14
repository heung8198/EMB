obj-m += step_motor_driver.o 
KDIR = ~/working/Raspbian/kernel

RESULT2 = step_motor

SRC2 = $(RESULT2).c

CCC = arm-linux-gnueabihf-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT2) $(SRC2)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf $(RESULT2)
