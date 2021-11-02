#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>


#define SCULL_IOC_MAGIC 'r'
#define SCULL_IOC_NR 11
#define SCULL_IOCRESET       _IO(SCULL_IOC_MAGIC,0)
#define SCULL_IOCSQUANTUM    _IOW(SCULL_IOC_MAGIC,1,int)
#define SCULL_IOCTQUANTUM    _IO(SCULL_IOC_MAGIC,3)
#define SCULL_IOCGQUANTUM    _IOR(SCULL_IOC_MAGIC,5,int)
#define SCULL_IOCQQUANTUM    _IO(SCULL_IOC_MAGIC,7)
#define SCULL_IOCXQUANTUM    _IOWR(SCULL_IOC_MAGIC,9,int)
#define SCULL_IOCHQUANTUM    _IO(SCULL_IOC_MAGIC,11)



int main()
{
	int fp, len;
	char bufferin[40];
	char bufferout[40] = "Scull driver sample example";
	
	fp = open("/dev/scull0", O_WRONLY);
	if(fp < 0) {
		printf("Failed to open the scull for writing!\n");
		exit(1);
	}

	len = write(fp, bufferout, strlen(bufferout)+1);
	if(len < 0) {
		printf("Error while writing to the device! \n");
		close(fp);
		exit(1);
	}
	printf("Wrote %d bytes to the device! \n", len);
	close(fp);


	fp = open("/dev/scull0", O_RDONLY);
	if( fp < 0) {
		printf("Error opening the device for reading \n");
		exit(1);
	}
	len = read(fp, bufferin, len);
	if (len < 0) {
		printf("Error reading from the device! \n");
		close(fp);
		exit(1);
	}
	printf("Read %d bytes from the device! \n", len);
	printf("%s \n", bufferin);

	//Test IOCTL call
	int quantum=3000;
	int result=0;

	ioctl(fp,SCULL_IOCSQUANTUM,&quantum);
	result= ioctl(fp,SCULL_IOCQQUANTUM);
	printf("Quantum value SCULL_IOCSQUANTUM %d\n",result);
	
	quantum=5000;
	ioctl(fp,SCULL_IOCTQUANTUM,quantum);
	result= ioctl(fp,SCULL_IOCQQUANTUM);
	printf("Quantum value SCULL_IOCTQUANTUM %d\n",result);


	quantum=1000;
	ioctl(fp,SCULL_IOCXQUANTUM,&quantum);
	result= ioctl(fp,SCULL_IOCQQUANTUM);
	printf("Quantum value SCULL_IOCXQUANTUM %d\n",result);


	quantum=500;
	ioctl(fp,SCULL_IOCHQUANTUM,quantum);
	result= ioctl(fp,SCULL_IOCQQUANTUM);
	printf("Quantum value SCULL_IOCHQUANTUM %d\n",result);

	ioctl(fp,SCULL_IOCRESET);
	result= ioctl(fp,SCULL_IOCQQUANTUM);
	printf("Quantum value SCULL_IOCHQUANTUM %d\n",result);

}
