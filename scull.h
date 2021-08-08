#ifndef _SCULL_H
#define _SCULL_H

#define SCULL_MAJOR 0;
#define SCULL_NUM_DEVICES 4;

struct scull_dev {
    struct cdev cdev; /*Char dev registration*/
    char* name; /*name*/
};

int scull_open(struct inode *inode, struct file *filp);

#endif /*SCULL_H*/




