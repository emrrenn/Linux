#include <linux/module.h>      
#include <linux/kernel.h> 
#include <linux/init.h>        
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>

#define mem_size	1024

uint8_t *kernel_buffer;
int condition = 0; /* Is okay to read */

static DECLARE_WAIT_QUEUE_HEAD(wait_queue);

static struct timer_list my_timer; 

static ssize_t emre_read(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t emre_write(struct file *file, const char __user *buf, size_t len, loff_t *off);
static int emre_open(struct inode *inode, struct file *file);
static int emre_release(struct inode *inode, struct file *file);
static unsigned int emre_poll (struct file *file, struct poll_table_struct *wait);
static void timer_callback(struct timer_list *t);

static void timer_callback(struct timer_list *t)
{
	condition = 1;
	wake_up_interruptible(&wait_queue);
	printk(KERN_INFO"Timer callback: Condition is set to 1 after waiting for 10s after write call.\n");
}

static int emre_open(struct inode *inode, struct file *file)
{
	/* Creating Physical Memory */
	if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0)
	{
		printk(KERN_INFO"Cannot allocate memory for device..\n");
		return -1;
	}
	printk(KERN_INFO"Device file opened.\n");
	return 0;	
}

static int emre_release(struct inode *inode, struct file *file)
{
	kfree(kernel_buffer);
	printk(KERN_INFO"Device file closed.\n");
	return 0;
}

static ssize_t emre_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
	if((__copy_from_user(kernel_buffer, buf, len)) != 0)
	{
		printk(KERN_INFO"Cannot write data from user to kernel space.");
		return -1;
	}
	mod_timer(&my_timer, jiffies + 10*HZ);
	printk(KERN_INFO"Data is written successfully. Timer is initialized.\n");
	return len;
}

static ssize_t emre_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
		
	if((__copy_to_user(buf, kernel_buffer, len)) != 0)
	{
		printk(KERN_INFO"Cannot read data from kernel to user space..");
		return -1;
	}
	printk(KERN_INFO"Data is read successfully\n");
	condition = 0;
	return mem_size;
}

static unsigned int emre_poll (struct file *file, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	poll_wait(file, &wait_queue, wait);
	
	if (condition) {
        	mask |= POLLIN;
    	}
	return mask;
}

dev_t poll_dev = 0;

static struct cdev poll_cdev;

static struct file_operations fops =
{
	.owner			=	THIS_MODULE,
	.read			=	emre_read,
	.write			=	emre_write,
	.open			=	emre_open,
	.release		=	emre_release,
	.poll			=	emre_poll
};


static struct class *poll_class;

static int __init emre_driver_init(void)
{
	/* Dynamically allocate a device number */
	if((alloc_chrdev_region(&poll_dev, 0, 1,"emre_poll_device")) < 0)
	{
		printk(KERN_INFO"Cannot allocate a device number..\n");
		return -1;	
	}
	
	/* Initialize the cdev structure with fops */
	cdev_init(&poll_cdev, &fops);

	/* Register cdev structure with VFS */
	if((cdev_add(&poll_cdev, poll_dev, 1)) < 0)
	{
		printk(KERN_INFO"Cannot add device to the system..\n");
		goto r_class;
	}

	/* create sysfs class */
	if((poll_class = class_create(THIS_MODULE, "emre_poll_device")) == NULL)
	{
		printk(KERN_INFO"Cannot create the struct class..\n");
		goto r_class;
	}

	/* create device node at /dev */
	if((device_create(poll_class, NULL, poll_dev, NULL, "emre_poll_device")) == NULL)
	{
		printk(KERN_INFO"Cannot create the device node..\n");
		goto r_device;
	}
	
	timer_setup(&my_timer, timer_callback, 0);
	
	printk(KERN_INFO"Device driver successfully inserted!\n");
	return 0;
	
	r_device:
			class_destroy(poll_class);
	
	r_class:
			unregister_chrdev_region(poll_dev, 1);
			return -1;
}

static void __exit emre_driver_exit(void)
{
	//Unregister and clean up the device 
	device_destroy(poll_class, poll_dev);

	class_destroy(poll_class);
	
	cdev_del(&poll_cdev);

	unregister_chrdev_region(poll_dev, 1);
	
	del_timer_sync(&my_timer);
	
	printk(KERN_INFO"Device driver successfully removed!\n");
}

module_init(emre_driver_init);
module_exit(emre_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EMRE");
MODULE_DESCRIPTION("A simple device driver for poll example");







