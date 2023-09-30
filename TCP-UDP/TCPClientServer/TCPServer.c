#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> //for close()

#include<netinet/in.h> //for the struct sockaddr_in
#include<arpa/inet.h> //for inet_addr()

#define PORT 5555

int main()
{
	// Create a socket
	int server_socket;	
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Error creatig a socket");
		exit(EXIT_FAILURE);
	}
	
	// Define the server address
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	// Bind the socket to the address and port
	if((bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)
	{
		perror("Error binding");
		exit(EXIT_FAILURE);
	}
	
	// Listen for incoming connections
    	if (listen(server_socket, 5) == -1) 
    	{
        	perror("Error listening");
        	exit(EXIT_FAILURE);
    	}
    	printf("Server listening on port %d...\n", PORT);
    	
    	// Accept a client connection
	int client_socket;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	
	client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    	if (client_socket == -1)
    	{
        	perror("Error accepting connection");
        	exit(EXIT_FAILURE);
    	}
	printf("Client connected.\n");

	// Communication with the client
    	char message[1024];
    	char response[] = "Hello Emre, welcome to the server.";
    	
       	memset(message, 0, sizeof(message));
        int bytes_received = recv(client_socket, message, sizeof(message), 0);
        if (bytes_received == -1)
        {
            	perror("Error receiving data");
            	exit(EXIT_FAILURE);
        }
     
        printf("Received: %s", message);
        
        // Send a response to the client
        send(client_socket, response, sizeof(response), 0);

    	// Close the sockets
    	close(client_socket);
    	close(server_socket);

    	return 0;
}
