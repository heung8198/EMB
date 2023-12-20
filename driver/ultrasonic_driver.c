#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple HC-SR04 Driver");

static dev_t my_device_nr;
static struct class* my_class;
static struct cdev my_device;

#define DRIVER_NAME "hcsr04"
#define DRIVER_CLASS "HCSR04ModuleClass"

#define TRIG_PIN 23 // 예시 트리거 핀 번호
#define ECHO_PIN 24 // 예시 에코 핀 번호

// 초음파 센서 거리 측정 로직
static int measure_distance(void) {
    ktime_t start_time, end_time;
    s64 time_diff;
    int distance;
    
    // 트리거 핀을 LOW 상태로 유지하여 센서를 초기화
    gpio_set_value(TRIG_PIN, 0);
    udelay(5); // 5us의 딜레이

    // 트리거 신호 보내기 (HIGH 상태로 10us 동안 유지)
    gpio_set_value(TRIG_PIN, 1);
    udelay(10);
    gpio_set_value(TRIG_PIN, 0);

    // 에코 신호가 HIGH가 될 때까지 기다림
    while (gpio_get_value(ECHO_PIN) == 0) {
        start_time = ktime_get();
    }

    // 에코 신호가 LOW가 될 때까지 기다림
    while (gpio_get_value(ECHO_PIN) == 1) {
        end_time = ktime_get();
    }

    // 시간 차이 계산 (에코 신호의 지속 시간)
    time_diff = ktime_to_us(ktime_sub(end_time, start_time));

    // 거리 계산 (cm 단위)
    distance = (int)time_diff / 58;

    // 센서가 다음 측정을 준비할 수 있도록 충분한 시간을 기다림
    udelay(200); // 200us의 딜레이

    // 거리 반환
    return distance;
}

static ssize_t driver_read(struct file* File, char* user_buffer, size_t count, loff_t* offs) {
    int to_copy, not_copied;
    int distance = measure_distance(); // 거리 측정

    to_copy = min(count, sizeof(distance));

    not_copied = copy_to_user(user_buffer, &distance, to_copy);

    return to_copy - not_copied;
}

//driver open
static int driver_open(struct inode* inode, struct file* file) {
    printk(KERN_INFO "UltraSonicDriver: Device opened\n");
    return 0;
}

//driver close
static int driver_close(struct inode* inode, struct file* file) {
    printk(KERN_INFO "UltraSonicDriver: Device closed\n");
    return 0;
}

// 파일 작업 구조체 정의
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
};

static int __init ModuleInit(void) {
    printk(KERN_INFO "HC-SR04 Module init\n");

    // 장치 번호 할당
    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk(KERN_ALERT "Could not allocate device number\n");
        return -1;
    }

    // 장치 클래스 생성
    if ((my_class = class_create(THIS_MODULE, ULTRASONIC_DRIVER_CLASS)) == NULL) {
        printk(KERN_ALERT "Could not create device class\n");
        unregister_chrdev_region(my_device_nr, 1);
        return -1;
    }

    // 장치 파일 생성
    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk(KERN_ALERT "Could not create device\n");
        class_destroy(my_class);
        unregister_chrdev_region(my_device_nr, 1);
        return -1;
    }

    // 장치 파일 초기화
    cdev_init(&my_device, &fops);
    if (cdev_add(&my_device, my_device_nr, 1) == -1) {
        printk(KERN_ALERT "Registering of device to kernel failed\n");
        device_destroy(my_class, my_device_nr);
        class_destroy(my_class);
        unregister_chrdev_region(my_device_nr, 1);
        return -1;
    }

    // GPIO 초기화
    if (gpio_request(TRIG_PIN, "rpi-gpio-trig") || gpio_request(ECHO_PIN, "rpi-gpio-echo")) {
        printk(KERN_ALERT "Cannot allocate GPIO\n");
        cdev_del(&my_device);
        device_destroy(my_class, my_device_nr);
        class_destroy(my_class);
        unregister_chrdev_region(my_device_nr, 1);
        return -1;
    }

    if (gpio_direction_output(TRIG_PIN, 0) || gpio_direction_input(ECHO_PIN)) {
        printk(KERN_ALERT "Cannot set GPIO direction\n");
        gpio_free(TRIG_PIN);
        gpio_free(ECHO_PIN);
        cdev_del(&my_device);
        device_destroy(my_class, my_device_nr);
        class_destroy(my_class);
        unregister_chrdev_region(my_device_nr, 1);
        return -1;
    }

    printk(KERN_INFO "HC-SR04 Driver loaded successfully\n");
    return 0;
}

static void __exit ModuleExit(void) {
    printk(KERN_INFO "HC-SR04 Module exit\n");

    // GPIO 정리
    gpio_free(TRIG_PIN);
    gpio_free(ECHO_PIN);

    // cdev 제거 및 장치 파일 정리
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);

    // 장치 번호 해제
    unregister_chrdev_region(my_device_nr, 1);
    printk(KERN_INFO "HC-SR04 Driver unloaded successfully\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
