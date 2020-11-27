//SPDX-License-Identifier: GPL-3.0
//
//my7seg/my7seg.c
//
//Copyright (C) 2020 Ryusuke Ihashi, Ryuichi Ueda
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>

MODULE_AUTHOR("Ryusuke Ihashi, Ryuichi Ueda");
MODULE_DESCRIPTION("driver for 7segment LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;
const u32 disp[17] = {
	63, //0 00111111
	6,  //1 00000110
	91, //2 01011011
	79, //3 01001111
	102,//4 01100110
	109,//5 01101101
	125,//6 01111101
	7,  //7 00000111
	127,//8 01111111
	111,//9 01101111
	119,//a 01110111
	124,//b 01111100
	88, //c 01011000
	94, //d 01011110
	121,//e 01111001
	113,//f 01110001
	128//.0 10000000

};
const u32 led[8] ={21, 20, 24, 25, 8, 16, 7, 23};

static ssize_t led_write(struct file* frip, const char* buf, size_t count, loff_t* pos){
	int i;
	int num;
	char c;
	u32 sum = 0;
	u32 reset = 0;
	if(copy_from_user(&c, buf, sizeof(char)))
		return -EFAULT;
	//printk(KERN_INFO "recived %c\n",c);
	if(c=='\n' || c==' ')return 1;
	else if(c >= 48 && c <= 57) num = (int)c - 48;
	else if(c >= 65 && c <= 70) num = (int)c - 55;
	else if(c >= 97 && c <= 102) num = (int)c - 87;
	else num = 16;

	for(i=0; i<8; i++){
		//reset += 1 << led[i];
		if((disp[num] >> i) & 1)
			sum += 1 << led[i];
		else
			reset += 1 << led[i];
	}	
	gpio_base[7] = sum;
	gpio_base[10] = reset;
	sum = 0;
	return 1;
}

static ssize_t sushi_read(struct file* flip, char* buf , size_t count, loff_t* pos){
	int size = 0;
	char sushi[]={'s', 'u', 's', 'h', 'i'};
	if(copy_to_user(buf+size, (const char *)sushi, sizeof(sushi))){	
		printk(KERN_ERR "sushi:copy_to_user failed.\n");
		return -EFAULT;
	}
	size += sizeof(sushi);
	return size;
		
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.read = sushi_read
};

static int __init init_mod(void){
	int i;
	int retval;
	u32 index;
	u32 shift;
	u32 mask;
	u32 reset=0;
	retval = alloc_chrdev_region(&dev, 0, 1, "my7seg");
	if(retval < 0){
		 printk(KERN_ERR "alloc_chrdev_region failed"); 
		 return 0; 
	}
	printk(KERN_INFO "%s is loaded. major:%d\n", __FILE__, MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major: %d, minor: %d\n", MAJOR(dev), MINOR(dev));
		return retval;
	}
	cls=class_create(THIS_MODULE, "my7seg");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed");
		return PTR_ERR(cls);
	}
	device_create(cls, NULL, dev, NULL, "my7seg%d", MINOR(dev));
	gpio_base = ioremap_nocache(0xfe200000, 0xA0);
	
	for(i=0; i<8; i++){
		index = led[i]/10;
		shift = (led[i]%10)*3;
		mask = ~(0x07 << shift);
		gpio_base[index]=(gpio_base[index]&mask) | (0x1<<shift);
		reset += 1<<led[i];
	}
	gpio_base[10] = reset;
	return 0;
}

static void __exit cleanup_mod(void){
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n", __FILE__, MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);
