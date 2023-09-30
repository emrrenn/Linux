#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "emre_ioctl.h"

int main()
{
	int speed;
	char *light;
	struct car emre_car;

	int fd;
	fd = open("/dev/emre_device", O_RDWR);
	
	if(fd < 0)
	{
		perror("Error message");
		return errno;
	}
	
	printf("Welcome to a simple user space application for Emre`s car.\n");
	printf("What would you like to do?\n");
	printf("-------1.Set Car-------\n");
	printf("-------2.Set Speed-------\n");
	printf("-------3.Get Speed-------\n");
	printf("-------4.Light Trigger-------\n");
	printf("-------5.Get Car-------\n");
	printf("-------6.Exit-------\n");
	
	
	while(1)
	{	
		char c;
		printf("Choose an option:");
		while ((c = getchar()) != '\n' && c != EOF)
		
		switch(c)
		{
			case('1'):
				printf("Who`s the owner? ");
				scanf(" %s", emre_car.owner);
				printf("What`s the model? ");
				scanf(" %s", emre_car.model);
				printf("What`s the max speed? ");
				scanf(" %d", &emre_car.speed);
				printf("Is the lights on or off? ");
				scanf(" %d", &emre_car.lights);
				
				ioctl(fd, SET_CAR, &emre_car);
				printf("Car successfully saved into kernel.\n");
				break;
				
			case('2'):
				printf("What`s the new max speed value? ");
				scanf("%d", &speed);
				
				ioctl(fd, SET_SPEED, &speed);
				printf("New Max Speed successfully saved into kernel.\n");
				break;
			
			case('3'):
				ioctl(fd, GET_SPEED, &speed);
				printf("Max Speed: %d\n", speed);
				break;
				
			case('4'):
				ioctl(fd, LIGHTS_ON_OFF);
				break;
			
			case('5'):
				ioctl(fd, CAR_INFO, &emre_car);
				printf("Emre`s car owner: %s\n", emre_car.owner);
				printf("Emre`s car model: %s\n", emre_car.model);
				printf("Emre`s car max speed: %dkm/h\n", emre_car.speed);
				light = (emre_car.lights == 1) ? "On" : "Off";
				printf("Emre`s car lights: %s\n", light);
				break;
				
			case('6'):
				printf("Bye!\n");
				exit(0);
				break;
			
			default:
				printf("Invalid option.\n");
				break;		
		}	
	}
	close(fd);
	return 0;
}
