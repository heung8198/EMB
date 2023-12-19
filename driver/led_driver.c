#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple gpio driver for setting a LED");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class* my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_led"
#define DRIVER_CLASS "MyModuleClass"

static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	int to_copy, not_copied, delta;
	int value;


	/* get amount of data to copy*/
	to_copy = min(count, sizeof(value));


	/*copy data to user*/
	not_copied = copy_from_user(&value, user_buffer, to_copy);

	/*setting the led*/

	switch (value) {
	case 0:
		gpio_set_value(20, 0);
		break;
	case 1:
		gpio_set_value(20, 1);
		break;
	default:
		printk("Invaild Input!\n");
		break;

	}

	delta = to_copy - not_copied;
	return delta;

}

static int driver_open(struct inode* device_file, struct file* instance) {
	printk("led_button - open was called\n");
	return 0;
}

static int driver_close(struct inode* device_file, struct file* instance) {
	printk("led_button - close was called\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	//.read = driver_read,
	.write = driver_write
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

	/*GPIo 20 init*/
	if (gpio_request(20, "rpi-gpio-20")) {
		printk("Can not allocate GPIO 20\n");
		goto AddError;
	}
	/*set Gpio 20 dircetion*/
	if (gpio_direction_output(20,0)) {
		printk("Can not set GPIO 20 to output!\n");
		goto Gpio20Error;
	}

	return 0;
Gpio20Error:
	gpio_free(20);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

static void __exit ModuleExit(void) {
	gpio_set_value(20, 0);
	gpio_free(20);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
