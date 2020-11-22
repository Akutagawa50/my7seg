#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>

MODULE_AUTHOR("Ryusuke Ihashi");
MODULE_DESCRIPTION("driver for 7segment LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

const u32 led[8] ={23, 24, 25, 8, 7, 16, 20, 21};

static ssize_t led_write(struct file* frip, const char* buf, size_t count, loff_t* pos){
	char c;
	if(copy_from_user(&c, buf, sizeof(char)))
		return -EFAULT;
	//printk(KERN_INFO "recived %c\n",c);
	if(c=='0'){
		gpio_base[10]=1<<23 | 1<<24 | 1<<25 | 1<<8 | 1<<7 | 1<<16 | 1<<20 | 1<<21;
	}
	else if(c=='1'){
		gpio_base[7]=1<<23 | 1<<24 | 1<<25 | 1<<8 | 1<<7 | 1<<16 | 1<<20 | 1<<21;
	}
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
	}
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
