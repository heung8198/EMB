#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Ultrasonic Sensor Driver");

/* Variables for device and device class */
static dev_t ultrasonic_dev;
static struct class *ultrasonic_class;
static struct device *ultrasonic_device;

#define DRIVER_NAME "ultrasonic_sensor"
#define TRIGGER_PIN 17
#define ECHO_PIN 18


static int distance;

static ssize_t distance_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
    int len = 0;
    char dist_str[10];

    // Trigger the sensor, Trigger Pin에 10us의 펄스 신호를 입력해서 거리 측정 시작
    gpio_set_value(TRIGGER_PIN, 1);
    udelay(10);
    gpio_set_value(TRIGGER_PIN, 0);

    // Wait for the echo signal, Echo Pin을 걸린 시간동안 펄스 신호 입력하여 거리 계산
    while (gpio_get_value(ECHO_PIN) == 0)
        ;
    long start_time = jiffies;

    while (gpio_get_value(ECHO_PIN) == 1) {
        if (time_after(jiffies, start_time + msecs_to_jiffies(20))) {
            // Timeout, break to avoid infinite loop
            printk(KERN_ALERT "Ultrasonic sensor read timeout\n");
            break;
        }
    }

    // Calculate distance based on the time difference
    // jiffies_to_msecs: 시스템이 경과된 시간을 msecs로 변환 후 측정
    unsigned long delta_time = jiffies_to_msecs(jiffies - start_time);
    distance = delta_time * 343 / 2000; // Speed of sound is 343 m/s

    len = snprintf(dist_str, sizeof(dist_str), "%d\n", distance);

    if (copy_to_user(buffer, dist_str, len))
        return -EFAULT;

    return len;
}

static struct file_operations ultrasonic_fops = {
    .read = distance_read,
};

static int __init ultrasonic_init(void) {
    int result;

    printk("Hello, kernel!\n");

    // Create device class, 형태 상이
    ultrasonic_class = class_create(THIS_MODULE, DRIVER_NAME);
    if (IS_ERR(ultrasonic_class)) {
        printk(KERN_ALERT "Failed to create class\n");
        goto cleanup_chrdev;
    }

    // Create device, 형태 상이
    ultrasonic_device = device_create(ultrasonic_class, NULL, ultrasonic_dev, NULL, DRIVER_NAME);
    if (IS_ERR(ultrasonic_device)) {
        printk(KERN_ALERT "Failed to create device\n");
        goto cleanup_class;
    }

    // Initialize character device
    cdev_init(&ultrasonic_cdev, &ultrasonic_fops);

    // Add character device to the system
    result = cdev_add(&ultrasonic_cdev, ultrasonic_dev, 1);
    if (result < 0) {
        printk(KERN_ALERT "Failed to add character device\n");
        goto cleanup_device;
    }

    // Request GPIO pins, trigger>echo 순으로 GPIO init
    result = gpio_request(TRIGGER_PIN, "ultrasonic_trigger");
    if (result < 0) {
        printk(KERN_ALERT "Failed to request trigger GPIO\n");
        return result;
    }

    result = gpio_request(ECHO_PIN, "ultrasonic_echo");
    if (result < 0) {
        printk(KERN_ALERT "Failed to request echo GPIO\n");
        goto cleanup_trigger;
    }

    // Set GPIO directions
    gpio_direction_output(TRIGGER_PIN, 0);
    gpio_direction_input(ECHO_PIN);

    // Allocate device number
    result = alloc_chrdev_region(&ultrasonic_dev, 0, 1, DRIVER_NAME);
    if (result < 0) {
        printk(KERN_ALERT "Failed to allocate device number\n");
        goto cleanup_gpio;
    }

    printk(KERN_INFO "Ultrasonic Sensor Driver initialized\n");
    return 0;

cleanup_device:
    device_destroy(ultrasonic_class, ultrasonic_dev);
cleanup_class:
    class_destroy(ultrasonic_class);
cleanup_chrdev:
    unregister_chrdev_region(ultrasonic_dev, 1);
cleanup_gpio:
    gpio_free(TRIGGER_PIN);
    gpio_free(ECHO_PIN);
cleanup_trigger:
    gpio_free(TRIGGER_PIN);
    return result;
}

static void __exit ultrasonic_exit(void) {
    cdev_del(&ultrasonic_cdev);
    device_destroy(ultrasonic_class, ultrasonic_dev);
    class_destroy(ultrasonic_class);
    unregister_chrdev_region(ultrasonic_dev, 1);
    gpio_free(TRIGGER_PIN);
    gpio_free(ECHO_PIN);
    printk(KERN_INFO "Ultrasonic Sensor Driver removed\n");
}

module_init(ultrasonic_init);
module_exit(ultrasonic_exit);
