#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>

char in_buf[20];
char kernelMsg[50];

int main() {
    int fd = open("/dev/emre_poll_device", O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return 1;
    }
   
    struct pollfd my_poll;
    my_poll.fd = fd;
    my_poll.events = POLLIN;
    
    printf("Enter the data for kernel: ");
    scanf(" %s", in_buf);
    write(fd, in_buf, sizeof(in_buf));
    
    int ret_val = poll(&my_poll, 1, -1);
    if (ret_val < 0) {
        perror("Poll failed.");
        close(fd);
        return EXIT_FAILURE;
    }
    
    if(my_poll.revents & POLLIN)
    {
    	read(fd, kernelMsg, sizeof(kernelMsg));
    	printf("Kernel Data: %s\n", kernelMsg);
    }
    close(fd);
    return 0;
}




