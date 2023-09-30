#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h>
#include<linux/mm.h>
#include<linux/string.h>

#define MEM_SIZE	(PAGE_SIZE * 4)

char *kernel_buffer;

/* Function Prototypes */
static int __init emre_driver_init(void);
static void __exit emre_driver_exit(void);

static int emre_open(struct inode *inode, struct file *file);
static int emre_release(struct inode *inode, struct file *file);
static int emre_mmap(struct file *file, struct vm_area_struct *vm_area);

char kernelMsg[] = "Hello from the kernel space!";

/* This holds the device number */
dev_t dev = 0;
/* Cdev variable  */
static struct cdev emre_cdev;

/* File operations of the driver */
static struct file_operations fops =
{
	.owner		=	THIS_MODULE,
	.open		=	emre_open,
	.release	=	emre_release,
	.mmap		=	emre_mmap		
};

/* sysfs class structure */
static struct class *emre_class;

/* Function Declarations */
static int emre_open(struct inode *inode, struct file *file)
{
	/* Creating Physical Memory */
	if((kernel_buffer = kmalloc(MEM_SIZE, GFP_KERNEL)) == 0)
	{
		printk(KERN_INFO"Cannot allocate memory for device..\n");
		return -1;
	}
	
	strncpy(kernel_buffer, kernelMsg, sizeof(kernelMsg));
	printk(KERN_INFO"Device file opened.\n");
	return 0;	
}

static int emre_release(struct inode *inode, struct file *file)
{
	kfree(kernel_buffer);
	printk(KERN_INFO"Device file closed.\n");
	return 0;
}

static int emre_mmap(struct file *file, struct vm_area_struct *vm_area)
{
	int ret;
	unsigned long size = vm_area->vm_end - vm_area->vm_start;
	unsigned long pfn = virt_to_phys(kernel_buffer)>>PAGE_SHIFT;
	
	ret = remap_pfn_range(vm_area, vm_area->vm_start, pfn, size, vm_area->vm_page_prot);
	if (ret < 0)
	{
    	pr_err("could not map the address area\n");
    	return -EIO;
	}
	
	return 0; 
}


static int __init emre_driver_init(void)
{
	/* Dynamically allocate a device number */
	if((alloc_chrdev_region(&dev, 0, 1,"mmap_device")) < 0)
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
    if((emre_class = class_create(THIS_MODULE, "mmap_device")) == NULL)
    {
		printk(KERN_INFO"Cannot create the struct class..\n");
		goto r_class;
    }

	/* create device node at /dev */
	if((device_create(emre_class, NULL, dev, NULL, "mmap_device")) == NULL)
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
