CC  = mipsel-openwrt-linux-gcc

all: fbutils

DEBUG = -O3 -g0

#COMMON_SOURCES =  "";

H_SOURCES = fbutils.h font.h

INCLUDE = -I.

WARNINGS= -Wcast-align -Wpacked -Wpadded -Wall

CCFLAGS = ${INCLUDE} ${DEBUG} ${WARNINGS} 

LDFLAGS =

#COMMON_OBJECTS = $(COMMON_SOURCES:.c=.o)

NANO_IP = 192.168.254.101

#fbutils: $(COMMON_OBJECTS) fbutils.c
fbutils:  fbutils.o
	$(CC)  $(LDFLAGS) fbutils.o -o fbutils

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

upload: jz_init_sram jz_test_gpio 
	scp jz_test_gpio jz_init_sram root@$(NANO_IP):

clean: 
	rm -f fbutils *.o

indent:
	indent -bad -bap -nbc -bl -nce -i2 --no-tabs --line-length120 $(COMMON_SOURCES) $(H_SOURCES)
