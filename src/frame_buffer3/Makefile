CC  = mipsel-openwrt-linux-gcc

all: init

DEBUG = -O3 -g0

#COMMON_SOURCES =  "";

#H_SOURCES = fbutils.h font.h

INCLUDE = -I.

WARNINGS= -Wcast-align -Wpacked -Wpadded -Wall

CCFLAGS = ${INCLUDE} ${DEBUG} ${WARNINGS} 

LDFLAGS =

#COMMON_OBJECTS = $(COMMON_SOURCES:.c=.o)

NANO_IP = 192.168.254.101

#fbutils: $(COMMON_OBJECTS) fbutils.c
fbutils:  init.o
	$(CC)  $(LDFLAGS) init.o -o init

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

clean: 
	rm -f init *.o

indent:
	indent -bad -bap -nbc -bl -nce -i2 --no-tabs --line-length120 $(COMMON_SOURCES) $(H_SOURCES)
