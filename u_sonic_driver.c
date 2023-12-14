#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>

#define TRIGGER_PIN 17   // Replace with your GPIO pin number
#define ECHO_PIN 18     // Replace with your GPIO pin number
#define DEVICE_NAME "hcsr04"

static dev_t dev_number;
static struct cdev hcsr04_cdev;
static struct class *cls;

static ktime_t echo_start;
static ktime_t echo_end;
static int distance;

static irqreturn_t echo_handler(int irq, void *dev_id) {
    if (gpio_get_value(ECHO_PIN)) {
        echo_start = ktime_get();
    } else {
        echo_end = ktime_get();
        distance = (int) ktime_to_us(ktime_sub(echo_end, echo_start)) / 58; // Convert time to distance
    }
    return IRQ_HANDLED;
}

static ssize_t hcsr04_read(struct file *file, char __user *userbuf, size_t count, loff_t *ppos) {
    char buf[20];
    int len = 0;

    // Trigger the HC-SR04 sensor
    gpio_set_value(TRIGGER_PIN, 1);
    udelay(10);
    gpio_set_value(TRIGGER_PIN, 0);

    // Wait for measurement to be available
    msleep(60);

    // Read the distance
    len = snprintf(buf, sizeof(buf), "%d\n", distance);
    if (len > count) 
        return -EFAULT;

    if (copy_to_user(userbuf, buf, len))
        return -EFAULT;

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = hcsr04_read,
};

static int __init hcsr04_init(void) {
    int ret;

    // Allocate character device
    if (alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME) < 0) {
        return -1;
    }

    // Create class
    cls = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(cls)) {
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }

    // Create device
    device_create(cls, NULL, dev_number, NULL, DEVICE_NAME);

    // Initialize char device
    cdev_init(&hcsr04_cdev, &fops);
    hcsr04_cdev.owner = THIS_MODULE;
    if (cdev_add(&hcsr04_cdev, dev_number, 1) < 0) {
        device_destroy(cls, dev_number);
        class_destroy(cls);
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }

    // Initialize GPIO
    gpio_request(TRIGGER_PIN, "Trigger");
    gpio_direction_output(TRIGGER_PIN, 0);
    gpio_request(ECHO_PIN, "Echo");
    gpio_direction_input(ECHO_PIN);

    // Set up interrupt handler
    if (request_irq(gpio_to_irq(ECHO_PIN), echo_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, DEVICE_NAME, NULL)) {
        cdev_del(&hcsr04_cdev);
        device_destroy(cls, dev_number);
        class_destroy(cls);
        unregister_chrdev_region(dev_number, 1);
        gpio_free(ECHO_PIN);
        gpio_free(TRIGGER_PIN);
        return -1;
    }

    printk(KERN_INFO "%s: Device initialized\n", DEVICE_NAME);
    return 0;
}

static void __exit hcsr04_exit(void) {
    // Free interrupt
    free_irq(gpio_to_irq(ECHO_PIN), NULL);

    // Remove char device
    cdev_del(&hcsr04_cdev);
    device_destroy(cls, dev_number);
    class_destroy(cls);
    unregister_chrdev_region(dev_number, 1);

    // Free GPIO
    gpio_free(ECHO_PIN);
    gpio_free(TRIGGER_PIN);

    printk(KERN_INFO "%s: Device removed\n", DEVICE_NAME);
}

module_init(hcsr04_init);
module_exit(hcsr04_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("HC-SR04 Driver");
