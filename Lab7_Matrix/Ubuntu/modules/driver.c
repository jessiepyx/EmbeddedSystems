#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jessie Peng <jessie_p@yeah.net>");
MODULE_DESCRIPTION("LED matrix driver for Lab7.");

#define DIN 17
#define CS 27
#define CLK 18

void write_byte(unsigned char b){
    unsigned char i, tmp;
    for (i=0; i<8; i++){
        tmp = (b & 0x80) > 0;
        b <<= 1;
        gpio_set_value(DIN, tmp);
        gpio_set_value(CLK, 1);
        gpio_set_value(CLK, 0);
    }
}

void write_word(unsigned char addr, unsigned char num){
    gpio_set_value(CLK, 0);
    gpio_set_value(CS, 1);
    gpio_set_value(CS, 0);
    write_byte(addr);
    write_byte(num);
    gpio_set_value(CS, 1);
}

void Matrix_render(unsigned char* tmp){
    int i;
    for (i=0; i<8; i++){
        write_word(i+1, tmp[i]);
    }
}

unsigned char digits[][8]={
    {0x1c, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c}, // 0
    {0x08, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1c}, // 1
    {0x1c, 0x22, 0x22, 0x04, 0x08, 0x10, 0x20, 0x3e}, // 2
    {0x1c, 0x22, 0x02, 0x0c, 0x02, 0x02, 0x22, 0x1c}, // 3
    {0x04, 0x0c, 0x14, 0x14, 0x24, 0x1e, 0x04, 0x04}, // 4
    {0x3e, 0x20, 0x20, 0x3c, 0x02, 0x02, 0x22, 0x1c}, // 5
    {0x1c, 0x22, 0x20, 0x3c, 0x22, 0x22, 0x22, 0x1c}, // 6
    {0x3e, 0x24, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08}, // 7
    {0x1c, 0x22, 0x22, 0x1c, 0x22, 0x22, 0x22, 0x1c}, // 8
    {0x1c, 0x22, 0x22, 0x22, 0x1e, 0x02, 0x22, 0x1c}, // 9
};

unsigned char LM[]={
    0x80, 0x80, 0x80, 0x91, 0x9b, 0xf5, 0x11, 0x11
};

void Matrix_clear(void){
    Matrix_render(LM);
}

void Matrix_init(void){
    write_word(0x09, 0x00);
    write_word(0x0a, 0x03);
    write_word(0x0b, 0x07);
    write_word(0x0c, 0x01);

    Matrix_clear();
}

#define BUFFERSIZE 128
#define DELAYTIME 1
unsigned char disp[BUFFERSIZE];
int head = 0, tail = 0;
static struct timer_list timer;

void Matrix_next_display(unsigned long);

void ptr_inc(int *ptr){
    *ptr = (*ptr + 1) % BUFFERSIZE;
}

static void timer_register(struct timer_list* ptimer){
    init_timer(ptimer);
    ptimer->data = DELAYTIME;
    ptimer->expires = jiffies + (DELAYTIME * HZ);
    ptimer->function = Matrix_next_display;
    add_timer(ptimer);
}

void disp_start(void){
    timer_register(&timer);
}

void Matrix_next_display(unsigned long data){
    if (head != tail){
        unsigned char *ptr = LM;
        unsigned char c = disp[head];
        if ('0' <= c && c <= '9'){
            ptr = digits[c - '0'];
        }
        Matrix_render(ptr);
        ptr_inc(&head);
        disp_start();
    }else{
        Matrix_clear();
    }
}

static int matrix_write(struct file *file, const char __user *buffer,
        size_t count, loff_t *ppos){
    int i;
    if (head == tail && count > 0){
        disp_start();
    }
    for (i=0; i<count; i++){
        ptr_inc(&tail);
        if (tail == head)
            ptr_inc(&head);
        disp[tail] = buffer[i];
    }
    return count;
}

static struct file_operations matrix_fops = {
    .owner = THIS_MODULE,
    .write = matrix_write,
    .llseek = noop_llseek
};

static struct miscdevice matrix_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "matrix",
    .fops = &matrix_fops
};

static int __init matrix_init(void){
    if (!gpio_is_valid(DIN) || !gpio_is_valid(CLK) || !gpio_is_valid(CS)){
        printk(KERN_INFO "GPIO_TEST: invalid GPIO\n");
        return -ENODEV;
    }
    misc_register(&matrix_misc_device);
    gpio_request(DIN, "sysfs");
    gpio_direction_output(DIN, 0);
    gpio_request(CS, "sysfs");
    gpio_direction_output(CS, 1);
    gpio_request(CLK, "sysfs");
    gpio_direction_output(CLK, 0);
    Matrix_init();
    printk(KERN_INFO"matrix device has been registed.\n");
    return 0;
}

static void __exit matrix_exit(void){
    misc_deregister(&matrix_misc_device);
    printk(KERN_INFO"matrix device has been unregisted.\n");
    del_timer(&timer);
}

module_init(matrix_init);
module_exit(matrix_exit);
