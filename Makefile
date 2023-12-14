obj-m += step_motor_driver.o
KDIR = ~/working/kernel

RESULT = step_motor

SRC = $(RESULT).c


CCC = aarch64-linux-gnu-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT) $(SRC)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf $(RESULT)
