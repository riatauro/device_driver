#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/mutex.h>

#include "scull.h"

int scull_major = SCULL_MAJOR;
int scull_minor =   0;
int scull_num_devices = SCULL_NUM_DEVICES;
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;
struct scull_dev *sculldev; /* device information */

struct file_operations fops = {
	.open = scull_open,
	.owner= THIS_MODULE,
	.read = scull_read ,
	.write = scull_write
};

static struct scull_qset* scull_follow(struct scull_dev* dev,int n) {
     struct scull_qset *data=dev->data;
     if(!data) {
         data=dev->data=kmalloc(sizeof(struct scull_qset),GFP_KERNEL);
	 if(data==NULL)
	     return NULL;
	 memset(data,0,sizeof(struct scull_qset));
     }
     while(n--) {
	     if(!data->next) {
		    data->next=kmalloc(sizeof(struct scull_qset),GFP_KERNEL);
		    if(data->next==NULL)
			  return NULL;
		    memset(data,0,sizeof(struct scull_qset));
	     }
	     data=data->next;
     }
     return data;

}

// consider fpos=50000
// itemsize - Total amount of bytes in one qset If quantum is of 4000 bytes and 10 array of pointers then itemsize=40000
// item = fpos/itemsize [One node of qset is 40000 Since fpos = 50000 so the position in the linkedlist will be 1 as only 40000 bytes are present in 1
// rest = fpos%itemsize [How many bytes should be ignored before starting to write data]
// arr_ptr = rest/quantum It is the position in the array of pointers. [(10000/4000) =2;]
// q_pos = rest % quantum . It is the position in the array from which write should start [(10000%4000)=2000]

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {


	printk(KERN_INFO "scull write called\n");
	printk("value of f_pos %d\n",*f_pos);
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum*qset;
	int item,arr_ptr,q_pos,rest;
	ssize_t retval = -ENOMEM;

	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	arr_ptr = rest / quantum;
	q_pos = rest % quantum;


        //if (down_interruptible(&dev->sem))
	  //   return -ERESTARTSYS;
	printk("value of item %d rest %d \n",item , rest);
	printk("value of arr_ptr %d q_pos %d \n",arr_ptr, q_pos);

	dptr = scull_follow(dev,item);
	if(dptr==NULL)
	    goto out;
	if(!dptr->data) {
	    dptr->data=kmalloc(qset*sizeof(char*),GFP_KERNEL);
	    if(!dptr->data)
	        goto out;
	    memset(dptr->data,0,qset*sizeof(char*));
	}
	if(!dptr->data[arr_ptr]) {
	    dptr->data[arr_ptr]=kmalloc(quantum,GFP_KERNEL);
	    if(!dptr->data[arr_ptr])
                goto out;
	 }

	if(count > quantum - q_pos)
	    count = quantum-q_pos;
	
	if (copy_from_user(dptr->data[arr_ptr]+q_pos, buf, count)) {
	    retval = -EFAULT;
	    printk("Return value %d\n",retval);
	    goto out;
	}

	*f_pos+=count;
	if(dev->size<*f_pos)
	    dev->size=*f_pos;
        retval=count;
	out:
	   //up(&dev->sem);
	   printk("Return outside value %d\n",retval);
	   return retval;

}


ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *fpos) {

	printk(KERN_INFO "scull read called\n");
	printk("count %d\n",count);
        printk("value of f_pos %d\n",*fpos);
	struct scull_dev *dev=filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset;
	int item , arr_pos, q_pos, rest;
	ssize_t retval= -ENOMEM;

	//if (down_interruptible(&dev->sem))
	//	return -ERESTARTSYS;
	if (*fpos >= dev->size)
		goto out;
	if (*fpos + count > dev->size)
		count = dev->size - *fpos;

	item= (long)*fpos / itemsize;
	rest = (long) *fpos % itemsize;
	arr_pos = rest / quantum;
	q_pos = rest % quantum;

	printk("value of item %d rest %d \n",item , rest);
	printk("value of arr_ptr %d q_pos %d \n",arr_pos, q_pos);

	dptr = scull_follow(dev,item);
	if(dptr==NULL || !dptr->data || !dptr->data[arr_pos]) {
	    printk("goto out");
	    goto out;
	}

	if(count>quantum  - q_pos)
	   count = quantum - q_pos;

	printk("count %d\n",count);

	if(copy_to_user(buf,dptr->data[arr_pos]+q_pos,count)) {
	   retval=-EFAULT;
	   printk("Return value %d\n",retval);
	   goto out;
	}

	*fpos+=count;
	retval=count;
	
out:
        //up(&dev->sem);
	printk("Return val %d\n",retval);
	return retval;

}




static int scull_setup_cdev(struct scull_dev *dev,int index) {
    int err=0;
    int devno=MKDEV(scull_major,scull_minor+index);
    cdev_init(&dev->cdev,&fops);
    dev->cdev.owner = THIS_MODULE;
    err=cdev_add(&dev->cdev,devno,1);
    if(err)
	printk(KERN_ERR "failure adding device\n");
    return 0;
}

int scull_trim(struct scull_dev* dev) {
    int qset=dev->qset;
    struct scull_qset *dataPtr,*next;
    int i;

    //iterate through scull data    
    for(dataPtr=dev->data;dataPtr!=NULL;dataPtr=next) {
	//if data is present
        if(dataPtr->data) {
	    //iterate through all elemets and delete
	    for(i=0;i<qset;i++)
                kfree(dataPtr->data[i]);
	    //free the array of pointers
	    kfree(dataPtr->data);
	    dataPtr->data=NULL;
	}
	//Move to the next set.
	next=dataPtr->next;
	kfree(dataPtr);
    }
    dev->qset=scull_qset;
    dev->quantum=scull_quantum;
    dev->size=0;
    dev->data=NULL;
    return 0;
}


void scull_cleanup_module(void)
{
    int i;
    dev_t devno = MKDEV(scull_major, scull_minor);
    if(sculldev) {
        for (i = 0; i < scull_num_devices; i++) {
            scull_trim(sculldev + i);
            cdev_del(&sculldev[i].cdev);
        }
        kfree(sculldev);

     }
    //unregister registered device number
    unregister_chrdev_region(devno, scull_num_devices);
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
	 result=register_chrdev_region(dev,scull_num_devices,"scull");

    } else {
	//Allocates major number dynamically
        result=alloc_chrdev_region(&dev,scull_minor,scull_num_devices,"scull");
	scull_major=MAJOR(dev);
	printk(KERN_INFO "Scull major number %d\n",scull_major);
    }
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    }
    sculldev=kmalloc(scull_num_devices*sizeof(struct scull_dev), GFP_KERNEL);

    if (!sculldev) {
	printk(KERN_INFO "no mem");
	result = -ENOMEM;
	goto fail;
    }

    memset(sculldev, 0,	scull_num_devices*sizeof(struct scull_dev));

    for(i=0;i<scull_num_devices;i++) {
	sculldev[i].quantum=scull_quantum;
	sculldev[i].qset=scull_qset;
	mutex_init(&sculldev[i].sem);
        scull_setup_cdev(&sculldev[i], i);	    
    }

    return 0;   
fail:
	printk(KERN_INFO "failed %d\n",scull_major);
        scull_cleanup_module();
        return result;
}

int scull_open(struct inode *i, struct file *filp)
{

    struct scull_dev *dev; 
    printk(KERN_INFO "Driver: open()\n");
    dev=container_of(i->i_cdev,struct scull_dev,cdev);
    filp->private_data=dev;
    if(filp->f_flags & O_ACCMODE==O_WRONLY) {
        scull_trim(dev);
    }
    return 0;
}



static void __exit scull_module_exit(void) {
    printk(KERN_INFO "Scull module exit\n");
    scull_cleanup_module();
}


module_init(scull_module_init);
module_exit(scull_module_exit);
MODULE_AUTHOR("Riana");
MODULE_DESCRIPTION("Simple Kernel Module to display messages on init and exit.");
MODULE_LICENSE("GPL");
