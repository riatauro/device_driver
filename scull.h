#ifndef _SCULL_H
#define _SCULL_H

#define SCULL_MAJOR 0;
#define SCULL_NUM_DEVICES 4;
#define SCULL_QSET 10;
#define SCULL_QUANTUM 4000;


struct scull_qset {
   void **data; //array of pointers refer scull memory image
   struct scull_qset *next;
};
struct scull_dev {
    struct scull_qset *data; /*Pointer to first quantum set*/
    int quantum; /*Current quantum size*/
    int qset; /*current array size*/
    unsigned long size; /* amount of data stored here*/
    unsigned int access_key; 
    struct mutex sem; /*mutual exclusion semaphore*/
    struct cdev cdev; /*Char dev registration*/
    char* name; /*name*/
};

int scull_open(struct inode *inode, struct file *filp);
ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
long scull_ioctl(struct file *filp,unsigned int cmd,unsigned long arg);
#endif /*SCULL_H*/


/** IOCTL  Definitions 
 *
 * Use 'r' as magic number
 * 
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": switch G and S atomically
 * H means "sHift": switch T and Q atomically
 */

#define SCULL_IOC_MAGIC 'r'
#define SCULL_IOC_NR 11
#define SCULL_IOCRESET       _IO(SCULL_IOC_MAGIC,0)
#define SCULL_IOCSQUANTUM    _IOW(SCULL_IOC_MAGIC,1,int)
#define SCULL_IOCTQUANTUM    _IO(SCULL_IOC_MAGIC,3)
#define SCULL_IOCGQUANTUM    _IOR(SCULL_IOC_MAGIC,5,int)
#define SCULL_IOCQQUANTUM    _IO(SCULL_IOC_MAGIC,7)
#define SCULL_IOCXQUANTUM    _IOWR(SCULL_IOC_MAGIC,9,int)
#define SCULL_IOCHQUANTUM    _IO(SCULL_IOC_MAGIC,11)




