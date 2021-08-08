#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>


#include "scull.h"

int scull_major = SCULL_MAJOR;
int scull_minor =   0;
int scull_devices = SCULL_NUM_DEVICES;
struct scull_dev *sculldev; /* device information */

struct file_operations fops = {
	.open = scull_open,
	.owner= THIS_MODULE,
};

static int scull_setup_cdev(struct scull_dev *dev,int index) {
    int devno=MKDEV(scull_major,scull_minor+index);
    cdev_init(&dev->cdev,&fops);
    cdev_add(&dev->cdev,devno,1);
    printk(KERN_INFO "device name %d %s\n",devno,dev->name);
    return 0;
}

static int __init scull_module_init(void){ 
    printk(KERN_INFO "Scull module init\n");
    dev_t dev=0;
    int result;
    int ret=0;
    int i=0;

    if(scull_major) {
	 //for static initialiation of major number using commandline
	 dev=MKDEV(scull_major,scull_minor);
	 result=register_chrdev_region(dev,scull_devices,"scull");

    } else {
	//Allocates major number dynamically
        result=alloc_chrdev_region(&dev,scull_minor,2,"scull");
	scull_major=MAJOR(dev);
	printk(KERN_INFO "Scull major number %d\n",scull_major);
    }
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    }
    sculldev=kmalloc(2*sizeof(struct scull_dev), GFP_KERNEL);

    if (!sculldev) {
	result = -ENOMEM;
        unregister_chrdev_region(dev, 2);
    }
    memset(sculldev, 0,2*sizeof(struct scull_dev));

    sculldev[0].name="device1";
    sculldev[1].name="device2";

    for(i=0;i<2;i++) {
        scull_setup_cdev(&sculldev[i], i);	    
    }
   

   return 0;
}

int scull_open(struct inode *i, struct file *f)
{

    struct scull_dev *dev; 
    printk(KERN_INFO "Driver: open()\n");
    dev=container_of(i->i_cdev,struct scull_dev,cdev);
    printk(KERN_INFO "Driver device name %s\n",dev->name);
    return 0;
}


static void __exit scull_module_exit(void) {
    int i=0;
    printk(KERN_INFO "Scull module exit\n");
    //Always unregister the major number assigned
    dev_t devno=MKDEV(scull_major,scull_minor);
    for(i=0;i<2;i++)
        cdev_del(&sculldev[i].cdev);
    unregister_chrdev_region(devno,2);
}


module_init(scull_module_init);
module_exit(scull_module_exit);
MODULE_AUTHOR("Riana");
MODULE_DESCRIPTION("Simple Kernel Module to display messages on init and exit.");
MODULE_LICENSE("GPL");
