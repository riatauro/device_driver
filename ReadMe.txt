*****************************************************************************VERSION 1*********************************************************************************************************************

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
 
**********************************************************************************************************************************************************************************************************   
 
