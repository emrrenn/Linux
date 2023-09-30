#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5555
#define BUFFER_SIZE 1024

int main() {

    int client_socket;
    struct sockaddr_in server_address = {0};
    

    // Create a UDP socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Send a message to the server
    char *mesg = "Hello Emre";
    int mesg_len = sendto(client_socket, (const char *)mesg, strlen(mesg), 0, (struct sockaddr *)&server_address, sizeof(server_address));
	if(mesg_len == -1)
	{
		perror("Error sending message");
		exit(EXIT_FAILURE);
	}
    
    // Close the client socket
    close(client_socket);

    return 0;
}

