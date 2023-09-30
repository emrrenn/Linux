#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> //for close()

#include<netinet/in.h> //for the struct sockaddr_in
#include <arpa/inet.h> //for inet_addr()




#define PORT 5555

int main()
{
	int client_socket;
	
	if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Errorcreatig a socket");
		exit(EXIT_FAILURE);
	}
	
	/*
		Define the server address
		
		struct sockaddr_in {
    		short            sin_family;   // e.g. AF_INET
    		unsigned short   sin_port;     // e.g. htons(PORT)
    		struct in_addr   sin_addr;     // see struct in_addr, below
    		char             sin_zero[8];  // zero this if you want to
		};
		
		sin_family: This field specifies the address family. For IPv4 addresses, it is typically set to AF_INET. 
		It helps the socket library identify that the address is in the IPv4 format.
		
		sin_port: This field holds the port number. Ports are used to differentiate between different services running on the same IP address. 
		It should be set using the htons function to ensure proper byte ordering (endianness) because network protocols use big-endian byte 
		order.
		
		sin_addr: This field holds the IP address. It is of type struct in_addr, which is itself a structure containing the IPv4 address. 
		The IP address can be set using the inet_addr function or by directly assigning values to the s_addr field of sin_addr.
		Loopback address 127.0.0.1, for communication within your local machine.	
	*/
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	/*
		Connect to the server
		
		int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
		
		connect - initiate a connection on a socket
		
		int sockfd: This is the socket descriptor. The connect() function will use this socket 
		to establish the connection to the server.
		
		const struct sockaddr *addr: This part of the code is responsible for specifying the server's address and port to which the client 
		wants to connect. It casts the server_addr structure (of type struct sockaddr_in) to the more generic struct sockaddr type 
		because connect() expects a pointer to a struct sockaddr.
		
		socklen_t addrlen: This specifies the size of the server address structure. 
		It tells the connect() function how many bytes it should 
		read from the memory location pointed to by struct sockaddr *addr. 
		By using sizeof(server_addr), you ensure that the connect() function reads the entire structure.
	*/
	
	if((connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)
	{
		perror("Error connecting to server");
		exit(EXIT_FAILURE);
	}
	
	printf("Successfully connected to server.\n");
	
	// Communication with the server
    	char message[1024];
   
        memset(message, 0, sizeof(message));
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        // Send the message to the server
        send(client_socket, message, strlen(message), 0);

        // Receive a response from the server
        recv(client_socket, message, sizeof(message), 0);
        printf("Server response: %s\n", message);
	
    	// Close the socket
    	close(client_socket);
    
    	return 0;
}
