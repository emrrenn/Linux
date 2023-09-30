#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>

#include "emre_ioctl.h"

#define mem_size	1024

static struct car emre_car;

uint8_t *kernel_buffer;

/* Function Prototypes */
static int __init emre_driver_init(void);
static void __exit emre_driver_exit(void);

static ssize_t emre_read(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t emre_write(struct file *file, const char __user *buf, size_t len, loff_t *off);
static int emre_open(struct inode *inode, struct file *file);
static int emre_release(struct inode *inode, struct file *file);
static long emre_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/* This holds the device number */
dev_t dev = 0;
/* Cdev variable  */
static struct cdev emre_cdev;

/* File operations of the driver */
static struct file_operations fops =
{
	.owner			=	THIS_MODULE,
	.read			=	emre_read,
	.write			=	emre_write,
	.open			=	emre_open,
	.release		=	emre_release,
	.unlocked_ioctl		=	emre_ioctl
};

/* sysfs class structure */
static struct class *emre_class;

/* Function Declarations */
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
		printk(KERN_INFO"Cannot write data from user to kernel space..");
		return -1;
	}
	printk(KERN_INFO"Data is written successfully");
	return len;
}

static ssize_t emre_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
	if((__copy_to_user(buf, kernel_buffer, len)) != 0)
	{
		printk(KERN_INFO"Cannot read data from kernel to user space..");
		return -1;
	}
	printk(KERN_INFO"Data is read successfully");
	return mem_size;
}


static long emre_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int32_t num_value;
	char *light;
	switch(cmd)
	{
		case SET_CAR:
			if(__copy_from_user(&emre_car, (struct car*)arg, sizeof(emre_car)))
				return -EFAULT;
			printk("Emre`s car owner: %s\n", emre_car.owner);
			printk("Emre`s car model: %s\n", emre_car.model);
			printk("Emre`s car max speed: %dkm/h\n", emre_car.speed);
			light = (emre_car.lights == 1) ? "On" : "Off";
			printk("Emre`s car lights: %s", light);
			break;	
			
		case SET_SPEED:
			if(__copy_from_user(&num_value, (int32_t*)arg, sizeof(num_value)))
				return -EFAULT;
			emre_car.speed = num_value;
			printk("Emre`s car speed set to: %dkm/h\n", emre_car.speed);
			break;
		
		case GET_SPEED:
			if(__copy_to_user((int32_t*)arg, &emre_car.speed ,sizeof(num_value)))
				return -EFAULT;
			printk("Emre's car speed successfully sent to user app.");
			break;
		
		case LIGHTS_ON_OFF:
			if(emre_car.lights == 0)
			{
				emre_car.lights = 1;
				printk("Lights turned on.");
			}
			else
			{
				emre_car.lights = 0;
				printk("Lights turned 1.");
			}
			break;
		
		case CAR_INFO:
			if(__copy_to_user((struct car*)arg, &emre_car, sizeof(emre_car)))
				return -EFAULT;
			printk("Emre's car information successfully sent to user app.");
			break;						
	}
	return 0;
}

static int __init emre_driver_init(void)
{
	/* Dynamically allocate a device number */
	if((alloc_chrdev_region(&dev, 0, 1,"emre_device")) < 0)
	{
		printk(KERN_INFO"Cannot allocate a device number..\n");
		return -1;	
	}
	
	/* Initialize the cdev structure with fops */
	cdev_init(&emre_cdev, &fops);

	/* Register cdev structure with VFS */
	if((cdev_add(&emre_cdev, dev, 1)) < 0)
	{
		printk(KERN_INFO"Cannot add device to the system..\n");
		goto r_class;
	}

	/* create sysfs class */
    if((emre_class = class_create(THIS_MODULE, "emre_device")) == NULL)
    {
		printk(KERN_INFO"Cannot create the struct class..\n");
		goto r_class;
    }

	/* create device node at /dev */
	if((device_create(emre_class, NULL, dev, NULL, "emre_device")) == NULL)
	{
		printk(KERN_INFO"Cannot create the device node..\n");
		goto r_device;
	}
	
	printk(KERN_INFO"Device driver successfully inserted!\n");
	return 0;
	
	r_device:
			class_destroy(emre_class);
	
	r_class:
			unregister_chrdev_region(dev, 1);
			return -1;
}

static void __exit emre_driver_exit(void)
{
	//Unregister and clean up the device 
	device_destroy(emre_class, dev);

	class_destroy(emre_class);
	
	cdev_del(&emre_cdev);

	unregister_chrdev_region(dev, 1);
	
	printk(KERN_INFO"Device driver successfully removed!\n");
}

module_init(emre_driver_init);
module_exit(emre_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EMRE");
MODULE_DESCRIPTION("A pseudo character device driver");
