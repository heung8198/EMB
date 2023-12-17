#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/delay.h>


/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple gpio driver for setting reading temperature and humidity");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class* my_class;
static struct cdev my_device;

#define DRIVER_NAME "DHT-11"
#define DRIVER_CLASS "MyMouldeClass"

#define DHT11_DATA_PIN 4


static ssize_t driver_read(struct file* File, char* user_buffer, size_t count, loff_t* offs) {
    int to_copy, not_copied;
    char sensor_data[5]; // 두 버튼의 상태를 저장하기 위해
    int i, j;
    uint8_t bit = 0;


    //send to dht-11 start signal
    gpio_direction_output(DHT11_DATA_PIN, 1);
    gpio_set_value(DHT11_DATA_PIN, 0);
    mdelay(18);
    gpio_set_value(DHT11_DATA_PIN, 1);
    udelay(40);
    gpio_direction_input(DHT11_DATA_PIN);

    
    //read data
    for (i = 0; i < 5; i++) {  // 5바이트 데이터
        for (j = 0; j < 8; j++) {  // 각 바이트당 8비트
            bit = 0;
            while (gpio_get_value(DHT11_DATA_PIN) == 0);  // low 상태 대기
            udelay(40);  // 데이터 비트의 길이 측정
            if (gpio_get_value(DHT11_DATA_PIN) == 1) {
                bit = 1;
                while (gpio_get_value(DHT11_DATA_PIN) == 1);  // high 상태 대기
            }
            sensor_data[i] <<= 1;
            sensor_data[i] |= bit;
        }
    }

    
    // 복사할 바이트 수를 가져옵니다 (button_state의 크기를 초과할 수 없음)
    to_copy = min(count, sizeof(sensor_data));


    // 사용자 버퍼로 데이터를 복사합니다.
    not_copied = copy_to_user(user_buffer, &sensor_data, to_copy);

    // 사용자 공간으로 복사된 바이트 수를 반환합니다.
    return to_copy - not_copied;
}

static int driver_open(struct inode* device_file, struct file* instance) {
    printk("DHT-11 - open was called\n");
    return 0;
}

static int driver_close(struct inode* device_file, struct file* instance) {
    printk("DHT-11 - close was called");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    
};

static int __init ModuleInit(void) {
	printk("Hello, kernel!\n");

	/*Allocate a device nr*/
	if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

	/*Create device class*/
	if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not created!\n ");
		goto ClassError;
	}
	/*create device file*/
	if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/*Initialize device file*/
	cdev_init(&my_device, &fops);

	/*Regisering device to kernel*/
	if (cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	/*GPIO 4 init*/
	if (gpio_request(4, "rpi-gpio-4")) {
		printk("Can not allocate GPIO 4\n");
		goto AddError;
	}
	
	/*Set GPIO 4 direction*/
	if (gpio_direction_input(DHT11_DATA_PIN)) {
		printk("Can not set GPIO 4 to input!\n");
		goto Gpio4Error;
	}
	
	return 0;
Gpio4Error:
	gpio_free(DHT11_DATA_PIN);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

static void __exit ModuleExit(void) {
	gpio_free(4);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
