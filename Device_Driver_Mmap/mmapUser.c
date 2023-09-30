#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MEM_SIZE	(sysconf(_SC_PAGESIZE) * 4)

int main()
{
	char *buf;	
	
	int fd = open("/dev/mmap_device", O_RDWR);
	if(fd < 0)
	{
		perror("Open");
		return 1;
	}
	
	struct stat st;
	void *map_addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map_addr == MAP_FAILED) 
	{
        	perror("mmap");
        	close(fd);
        	return 1;
        }
        
        //Read from file
        printf("Stored string from the device: %s\n", (char *)map_addr);
	
	//Write to file
	printf("Enter a string to be copied: ");
	scanf("%s", buf);
	strncpy((char *)map_addr, buf, MEM_SIZE);
	
	//Read from file
	printf("Copied string from the device: %s\n", (char *)map_addr);
	
	munmap(map_addr, MEM_SIZE);
	close(fd);
	
	return 0;
}
