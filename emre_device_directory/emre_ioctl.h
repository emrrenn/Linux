#ifndef EMRE_IOCTL
#define EMRE_IOCTL

/* Magic number is specific to device file which it was implemented - 'a'*/
/* Unique identifier distinguish one control command from another within a specific device driver - 'b' , 'c' */
#define SET_CAR			_IOW('a','a', struct car*)
#define SET_SPEED 		_IOW('a','b', int32_t*)
#define GET_SPEED 		_IOR('a','c', int32_t*)
#define LIGHTS_ON_OFF		_IO('a','d')
#define CAR_INFO		_IOR('a','e', struct car*)

struct car{
	char owner[30];
	char model[30];	   
	int32_t speed;
	int32_t lights; /* 0 = OFF - 1 = ON */
};
#endif
