#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/ioctl.h>

static char *defaultfbdevice = "/dev/fb1";

unsigned char screen[640*480];

int main(){
	int fb;
	char c;
	fb = open(defaultfbdevice, O_RDWR);
	if (fb == -1)
    	perror("open:");
	assert(fb != -1);
	unsigned int i;
  
	for (i = 0; i < 320*240; i = i+2){
	  	screen[i] = 0x1;
	}
  
	for (i = 1; i < 320*240; i = i+2){ 
	  	screen[i] = 0x0;
	}
	
	write(fb, screen, 320*240);	
	close(fb);
	exit( 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER
#define CONFIGURE_EXTRA_TASK_STACKS 1900
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_POSIX_THREADS         1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES 1
#define CONFIGURE_INIT
