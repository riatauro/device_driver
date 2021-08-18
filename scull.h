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
#endif /*SCULL_H*/




