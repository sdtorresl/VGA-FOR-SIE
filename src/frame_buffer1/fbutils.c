#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/vt.h>
#include <linux/kd.h>
#include <linux/fb.h>

#include "font.h"
#include "fbutils.h"

union multiptr {
	unsigned char *p8;
	unsigned short *p16;
	unsigned long *p32;
};

static int con_fd, last_vt = -1;
static struct fb_fix_screeninfo fix;
static struct fb_var_screeninfo var;
static unsigned char *fbuffer;
static unsigned char **line_addr;
static int fb_fd=0;
static int bytes_per_pixel;
static unsigned colormap [256];
__u32 xres, yres;

static char *defaultfbdevice = "/dev/fb1";
static char *defaultconsoledevice = "/dev/tty";
static char *fbdevice = NULL;
static char *consoledevice = NULL;

int open_framebuffer(void)
{
	struct vt_stat vts;
	char vtname[128];
	int fd, nr;
	unsigned y, addr;
	unsigned int dir, i;
	unsigned char dato;

	if ((fbdevice = getenv ("TSLIB_FBDEVICE")) == NULL) {
		fbdevice = defaultfbdevice;
	}

	if ((consoledevice = getenv ("TSLIB_CONSOLEDEVICE")) == NULL)
		consoledevice = defaultconsoledevice;

	if (strcmp (consoledevice, "none") != 0) {
		sprintf (vtname,"%s%d", consoledevice, 1);
        fd = open (vtname, O_WRONLY);
        if (fd < 0) {
       		perror("open consoledevice");
        	return -1;
        }

		if (ioctl(fd, VT_OPENQRY, &nr) < 0) {
        	perror("ioctl VT_OPENQRY");
        	return -1;
        }
        
        close(fd);

        sprintf(vtname, "%s%d", consoledevice, nr);

       	con_fd = open(vtname, O_RDWR | O_NDELAY);
        
        if (con_fd < 0) {
        	perror("open tty");
        	return -1;
        }

       	if (ioctl(con_fd, VT_GETSTATE, &vts) == 0)
        	last_vt = vts.v_active;

        if (ioctl(con_fd, VT_ACTIVATE, nr) < 0) {
        	perror("VT_ACTIVATE");
        	close(con_fd);
        	return -1;
        }

        if (ioctl(con_fd, VT_WAITACTIVE, nr) < 0) {
        	perror("VT_WAITACTIVE");
        	close(con_fd);
        	return -1;
        }

        if (ioctl(con_fd, KDSETMODE, KD_GRAPHICS) < 0) {
        	perror("KDSETMODE");
        	close(con_fd);
        	return -1;
        }

	}

	fb_fd = open(fbdevice, O_RDWR);
	if (fb_fd == -1) {
		perror("open fbdevice");
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix) < 0) {
		perror("ioctl FBIOGET_FSCREENINFO");
		close(fb_fd);
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &var) < 0) {
		perror("ioctl FBIOGET_VSCREENINFO");
		close(fb_fd);
		return -1;
	}
	xres = var.xres;
	yres = var.yres;

	fbuffer = mmap(NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fb_fd, 0);
	if (fbuffer == (unsigned char *)-1) {
		perror("mmap framebuffer");
		close(fb_fd);
		return -1;
	}
	
	//borrado de la pantalla
	for (i = 0; i < fix.smem_len; i++){
		dir = i & 0x3FF;
		dato =( 0 + ((i&0x1FC00)>>9) )&0xFF;
		
		fbuffer[dir] = dato;
	}

	bytes_per_pixel = (var.bits_per_pixel + 7) / 8;
	line_addr = malloc (sizeof (__u32) * var.yres_virtual);
	addr = 0;
	for (y = 0; y < var.yres_virtual; y++, addr += fix.line_length)
		line_addr [y] = fbuffer + addr;
		
	printf("Opening framebuffer. Interesting parameters\n");
	printf("bytes per pixel %d\n", bytes_per_pixel);
	printf("y-resolution %d\n", var.yres_virtual);
	printf("x-resolution %d\n", fix.line_length);
	printf("FB memory %d\n\n", fix.smem_len);

	return 0;
}

void close_framebuffer(void)
{
	munmap(fbuffer, fix.smem_len);
	close(fb_fd);


	if(strcmp(consoledevice,"none")!=0) {
	
        	if (ioctl(con_fd, KDSETMODE, KD_TEXT) < 0)
        	        perror("KDSETMODE");

        	if (last_vt >= 0)
        	        if (ioctl(con_fd, VT_ACTIVATE, last_vt))
        	                perror("VT_ACTIVATE");

        	close(con_fd);
	}

        free (line_addr);
}


void put_char(int x, int y, int c, int colidx)
{
	int i,j,bits;

	for (i = 0; i < font_vga_8x8.height; i++) {
		bits = font_vga_8x8.data [font_vga_8x8.height * c + i];
		for (j = 0; j < font_vga_8x8.width; j++, bits <<= 1)
			if (bits & 0x80)
				pixel (x + j, y + i, colidx);
	}
}

void put_string(int x, int y, char *s, unsigned colidx)
{
	int i;
	for (i = 0; *s; i++, x += font_vga_8x8.width, s++)
		put_char (x, y, *s, colidx);
}

void put_string_center(int x, int y, char *s, unsigned colidx)
{
	size_t sl = strlen (s);
        put_string (x - (sl / 2) * font_vga_8x8.width,
                    y - font_vga_8x8.height / 2, s, colidx);
}

void setcolor(unsigned colidx, unsigned value)
{
	unsigned res;
	unsigned short red, green, blue;
	struct fb_cmap cmap;

#ifdef DEBUG
	if (colidx > 255) {
		fprintf (stderr, "WARNING: color index = %u, must be <256\n",
			 colidx);
		return;
	}
#endif

	switch (bytes_per_pixel) {
	default:
	case 1:
		res = colidx;
		red = (value >> 8) & 0xff00;
		green = value & 0xff00;
		blue = (value << 8) & 0xff00;
		cmap.start = colidx;
		cmap.len = 1;
		cmap.red = &red;
		cmap.green = &green;
		cmap.blue = &blue;
		cmap.transp = NULL;

        	if (ioctl (fb_fd, FBIOPUTCMAP, &cmap) < 0)
        	        perror("ioctl FBIOPUTCMAP");
		break;
	case 2:
	case 4:
		red = (value >> 16) & 0xff;
		green = (value >> 8) & 0xff;
		blue = value & 0xff;
		res = ((red >> (8 - var.red.length)) << var.red.offset) |
                      ((green >> (8 - var.green.length)) << var.green.offset) |
                      ((blue >> (8 - var.blue.length)) << var.blue.offset);
	}
        colormap [colidx] = res;
}

void pixel (int x, int y, unsigned colidx)
{
	unsigned xormode;
	union multiptr loc;

	if ((x < 0) || ((__u32)x >= var.xres_virtual) ||
	    (y < 0) || ((__u32)y >= var.yres_virtual))
		return;

	xormode = colidx & XORMODE;
	colidx &= ~XORMODE;

#ifdef DEBUG
	if (colidx > 255) {
		fprintf (stderr, "WARNING: color value = %u, must be <256\n",
			 colidx);
		return;
	}
#endif

	
	unsigned int pix_addr = line_addr [y] + x * bytes_per_pixel - line_addr[0];	//dirección que verá la fpga
	unsigned int color = ( colormap [colidx] + ((pix_addr&0x1FC00)>>9) )&0xFF;
	
	
	//dir = i & 0x3FF
	loc.p8 =  line_addr[0] +  (pix_addr & 0x3FF);
	*loc.p8 = color;
}
/*****************************TEST FRAMEBUFFER*********************************/
static int palette [] = \
{
	0,1
};
#define NR_COLORS (sizeof (palette) / sizeof (palette [0]))

int main()
{
	int i,j,k;

	printf("Opening framebuffer (a)...\n\n");

	if (open_framebuffer()) {
		close_framebuffer();
		exit(1);
		printf("Framebuffer could not be opened!!\n\n");
	}
	

	for (i = 0; i < NR_COLORS; i++)
		setcolor (i, palette [i]);
    
	put_string_center (xres / 2, yres *0.2,
	   "Nicolas David Arias Sosa", 1);
	put_string_center (xres / 2, yres *0.3,
	   "Gustavo Adolfo Valbuena Reyes", 1);
	put_string_center (xres / 2, yres *0.4,
	   "Sergio Daniel Torres", 1);
   	put_string_center (xres / 2, yres *0.6,
	   "Universidad Nacional de Colombia", 1);
	put_string_center (xres / 2, yres *0.6 + 60,
	   "Driver VGA", 1);
    put_string_center (xres / 2, yres *0.6 + 80,
	   "2011-II - Plataforma SIE", 1);
  
	close_framebuffer();
	printf("Framebuffer closed...\n\n");
}

