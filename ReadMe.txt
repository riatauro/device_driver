*****************************************************************************VERSION1******************************************************************************

Description :
A simple program to load a driver. Allocate major number for the driver.
Assign fops to each device and call open call on each device


Steps 1 : Load the module 
          insmod scull_driver.ko

Step  2 : cat /proc/devices [Check major number] 
          o/p- 236 scull

Step 3  : Create device files using mknod
          mknod /dev/scull0 C 236 0
          mknod /dev/scull1 C 236 1

Step 4  : cat /dev/scull0 [open for device 1 called]
          cat /dev/scull1 [open for device 2 called]


Logs :
[13196.879112] Scull module init
[13196.879114] Scull major number 236
[13196.879116] device name 247463936 device1
[13196.879117] device name 247463937 device2
[13525.243072] Driver: open()
[13525.243079] Driver device name device1
[13527.088332] Driver: open()
[13527.088335] Driver device name device2
[13575.689886] Scull module exit
 
********************************************************************************************************************************************************************
Version 1.1

In scull, each device is a linked list of pointers, each of which points to a Scull_Dev structure. Each such structure can refer, by default, to at most fourty thousand bytes, 
through an array of intermediate pointers. The released source uses an array of 10 pointers to areas of 4000 bytes.

Every device is represented by a scull_dev. Scull_dev has a linked list node call q_set which is linked list node with a array of pointers (qset) of 1 quantum each(4000).

Read operation:

// consider fpos=50000
// itemsize - Total amount of bytes in one qset If quantum is of 4000 bytes and 10 array of pointers then itemsize=40000
// item = fpos/itemsize [One node of qset is 40000 Since fpos = 50000 so the position in the linkedlist will be 1 as only 40000 bytes are present in 1
// rest = fpos%itemsize [How many bytes should be ignored before starting to write data]
// arr_ptr = rest/quantum It is the position in the array of pointers. [(10000/4000) =2;]
// q_pos = rest % quantum . It is the position in the array from which write should start [(10000%4000)=2000]

1) Reach till the dptr->data[arr_ptr]. If it is not null use copytouser(to,from,count)


Write operation

1) Similar to read operation except allocate memory till you reach the offset.
2) qptr = dev->data [Data type : struct scull_qset]
3) qptr->data [Data type: char* *10 (Array of 10 pointers)]
3) qptr->data[q_pos] [Data size: 4000 (one quantum);
4) copy_from_user(to,from,count)


unsigned long copy_to_user(void __user *to,const void *from,unsigned long count);
unsigned long copy_from_user(void *to,const void __user *from,unsigned long count);

*********************************************************************************************************************************************************************************

Version 1.2 - IOCTL calls

*********************************************************************************************************************************************************************************
