#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
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
static struct cdev ultrasonic_cdev;

#define DRIVER_NAME "ultrasonic_sensor"
#define TRIGGER_PIN 17
#define ECHO_PIN 18

static int distance;

static ssize_t distance_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
    int len = 0;
    char dist_str[10];
    long start_time;  // 선언 위치 변경
    unsigned long delta_time;  // 선언 위치 변경

    // Trigger the sensor, Trigger Pin에 10us의 펄스 신호를 입력해서 거리 측정 시작
    gpio_set_value(TRIGGER_PIN, 1);
    udelay(10);
    gpio_set_value(TRIGGER_PIN, 0);

    // Wait for the echo signal, Echo Pin을 걸린 시간동안 펄스 신호 입력하여 거리 계산
    while (gpio_get_value(ECHO_PIN) == 0)
        ;
    start_time = jiffies;

    while (gpio_get_value(ECHO_PIN) == 1) {
        if (time_after(jiffies, start_time + msecs_to_jiffies(20))) {
            // Timeout, break to avoid infinite loop
            printk(KERN_ALERT "Ultrasonic sensor read timeout\n");
            break;
        }
    }

    // Calculate distance based on the time difference
    delta_time = jiffies_to_msecs(jiffies - start_time);
    distance = delta_time * 343 / 2000; // Speed of sound is 343 m/s

    len = snprintf(dist_str, sizeof(dist_str), "%d\n", distance);

    if (copy_to_user(buffer, dist_str, len))
        return -EFAULT;

    return len;
}

// ... [file_operations 및 ultrasonic_init 함수는 변경하지 않음] ...

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
