#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5555
#define BUFFER_SIZE 1024

int main()
{
	int server_socket;
    struct sockaddr_in server_addr = {0};
    

    // Create a UDP socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    int res = bind(server_socket, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    if(res == -1)
    {
    	perror("Error binding socket");
    	close(server_socket);
    	exit(EXIT_FAILURE);
    }
    
    char buffer[BUFFER_SIZE] = {0};
    int mesg_len = recvfrom(server_socket, (char *)buffer, BUFFER_SIZE, 0, NULL, 0);
    if(mesg_len == -1)
    {
    	perror("Error receiving message");
    	exit(EXIT_FAILURE);
    }
    
    printf("Client message: %s\n", buffer);
    
    close(server_socket);
    return 0;
}
