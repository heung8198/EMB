#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple servo motor driver");

static dev_t my_device_nr;
static struct class* my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_servo"
#define DRIVER_CLASS "MyModuleClass_Servo"

// GPIO pin for servo motor
static int gpio_servo = 18; // 예시로 GPIO 18 사용

static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
    char buffer[10];
    int pulse_width;

    if (copy_from_user(buffer, user_buffer, count)) {
        return -EFAULT;
    }

    sscanf(buffer, "%d", &pulse_width);

    // PWM 신호 생성 (간단한 예시)
    gpio_set_value(gpio_servo, 1);
    udelay(pulse_width); // 펄스 폭에 따라 HIGH 상태 유지
    gpio_set_value(gpio_servo, 0);
    msleep(20 - (pulse_width / 1000)); // 20ms 주기 유지

    return count;
}

static int driver_open(struct inode* inode, struct file* file) {
    printk(KERN_INFO "ServoMotorDriver: Device opened\n");
    return 0;
}

static int driver_close(struct inode* inode, struct file* file) {
    printk(KERN_INFO "ServoMotorDriver: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .write = driver_write
};

static int __init ModuleInit(void) {
    printk("Servo Motor Module loaded\n");

    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("Device Nr. could not be allocated!\n");
        return -1;
    }

    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Device class can not be created!\n");
        goto ClassError;
    }

    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk("Can not create device file!\n");
        goto FileError;
    }

    cdev_init(&my_device, &fops);

    if (cdev_add(&my_device, my_device_nr, 1) == -1) {
        printk("Registering of device to kernel failed!\n");
        goto AddError;
    }

    if (gpio_request(gpio_servo, "rpi-gpio-servo")) {
        printk(KERN_ALERT "Cannot allocate GPIO %d\n", gpio_servo);
        goto GpioError;
    }
    if (gpio_direction_output(gpio_servo, 0)) {
        printk(KERN_ALERT "Cannot set GPIO %d to output\n", gpio_servo);
        goto GpioDirError;
    }

    return 0;

GpioDirError:
    gpio_free(gpio_servo);
GpioError:
AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev_region(my_device_nr, 1);
    return -1;
}

static void __exit ModuleExit(void) {
    gpio_set_value(gpio_servo, 0);
    gpio_free(gpio_servo);
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);
    printk("Servo Motor Module unloaded\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
