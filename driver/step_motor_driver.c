#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h> // msleep을 사용하기 위한 헤더 파일

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple stepper motor driver using ULN2003");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class* my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_stepper"
#define DRIVER_CLASS "MyModuleClass_Stepper"
#define BACKWARD 2
#define FORWARD 1
#define STEPS_PER_REVOLUTION 1000 //180 

static int step_sequence[] = {0b0001, 0b0010, 0b0100, 0b1000}; // Basic step sequence
static int step_index = 0; // Current step index

// GPIO pin setup for ULN2003
static int gpio_pins[] = {17, 27, 22, 5}; // Example GPIO pin numbers

static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	int to_copy, not_copied, delta, mode;
	to_copy = min(count, sizeof(mode));
	
	not_copied = copy_from_user(&mode, user_buffer, to_copy);
	if (mode == FORWARD) {
        for (int step = 0; step < STEPS_PER_REVOLUTION; step++) {
            step_index = (step_index + 1) % 4;
            for (int i = 0; i < 4; i++) {
                gpio_set_value(gpio_pins[i], (step_sequence[step_index] & (1 << i)) != 0);
            }
            		// 추가적인 딜레이를 넣을 수 있음
			msleep(10); // 10ms 딜레이
        }
    } else if (mode == BACKWARD) {
        for (int step = 0; step < STEPS_PER_REVOLUTION; step++) {
            step_index = (step_index - 1 + 4) % 4;
            for (int i = 0; i < 4; i++) {
                gpio_set_value(gpio_pins[i], (step_sequence[step_index] & (1 << i)) != 0);
            }
           		 // 추가적인 딜레이를 넣을 수 있음
			msleep(10); // 10ms 딜레이
        }
    }
    delta = to_copy - not_copied;
    return delta;	
}

//driver open
static int driver_open(struct inode* inode, struct file* file) {
    printk(KERN_INFO "StepperMotorDriver: Device opened\n");
    return 0;
}

//driver close
static int driver_close(struct inode* inode, struct file* file) {
    printk(KERN_INFO "StepperMotorDriver: Device closed\n");
    return 0;
}

// 파일 작업 구조체 정의
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .write = driver_write
};

static int __init ModuleInit(void) {
    printk("Stepper Motor Module loaded\n");

    // Device and GPIO initialization logic
    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk(KERN_ALERT "StepperMotorDriver: Couldn't allocate device number\n");
        return -1;
    }
	
    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk(KERN_ALERT "StepperMotorDriver: Couldn't create device class\n");
        unregister_chrdev_region(my_device_nr, 1);
        return -1;
    }

    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk(KERN_ALERT "StepperMotorDriver: Couldn't create device file\n");
        class_destroy(my_class);
        unregister_chrdev_region(my_device_nr, 1);
        return -1;
    }

    // Initialize device file
    cdev_init(&my_device, &fops);

    if (cdev_add(&my_device, my_device_nr, 1) == -1) {
        printk(KERN_ALERT "StepperMotorDriver: Registering of device to kernel failed\n");
        device_destroy(my_class, my_device_nr);
        class_destroy(my_class);
        unregister_chrdev_region(my_device_nr, 1);
        return -1;
    }

    // Additional GPIO initialization specific to stepper motor
	int i1, j1;
    for (i1 = 0; i1 < 4; i1++) {
        if (gpio_request(gpio_pins[i1], "rpi-gpio-stepper")) {
            printk(KERN_ALERT "StepperMotorDriver: Cannot allocate GPIO %d\n", gpio_pins[i1]);
            // Cleanup in case of error
            for (j1 = 0; j1 < i1; j1++) {
                gpio_free(gpio_pins[j1]);
            }
		
            cdev_del(&my_device);
            device_destroy(my_class, my_device_nr);
            class_destroy(my_class);
            unregister_chrdev_region(my_device_nr, 1);
            return -1;
        }

        if (gpio_direction_output(gpio_pins[i1], 0)) {
            printk(KERN_ALERT "StepperMotorDriver: Cannot set GPIO %d to output\n", gpio_pins[i1]);
            // Cleanup in case of error
            for (j1 = 0; j1 <= i1; j1++) {
                gpio_free(gpio_pins[j1]);
            }
		
            cdev_del(&my_device);
            device_destroy(my_class, my_device_nr);
            class_destroy(my_class);
            unregister_chrdev_region(my_device_nr, 1);
            return -1;
        }
    }
    return 0;
}

static void __exit ModuleExit(void) {
    // GPIO cleanup logic
	int i2;
    for (i2 = 0; i2 < 4; i2++) {
        gpio_set_value(gpio_pins[i2], 0);
        gpio_free(gpio_pins[i2]);
    }

    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);

    printk("Stepper Motor Module unloaded\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
