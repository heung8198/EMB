
obj-m += temperature_humidity_driver.o

KDIR = ~/working/kernel

RESULT1 = temperature_humidity

SRC1 = $(RESULT1).c

CCC = aarch64-linux-gnu-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT1) $(SRC1)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf $(RESULT1)
