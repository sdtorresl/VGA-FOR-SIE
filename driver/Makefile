EXTRA_CFLAGS += -Wall
CC            = mipsel-openwrt-linux-gcc
OPENWRT_BASE  = ~/trunk
KERNEL_SRC    = $(OPENWRT_BASE)/build_dir/linux-xburst_qi_lb60/linux-2.6.37.6/
CROSS_COMPILE = mipsel-openwrt-linux-

obj-m +=  vgamonochr.o
all: driver

driver:
	make -C $(KERNEL_SRC) M=$(PWD) ARCH=mips CROSS_COMPILE=$(CROSS_COMPILE) modules
clean:
	make -C $(KERNEL_SRC) M=$(PWD) ARCH=mips CROSS_COMPILE=$(CROSS_COMPILE) clean
	rm -rf *.o main.o main irq.ko Modules.symvers irq_main

main: main.o

PREPROCESS.c = $(CC) $(CFLAGS) $(TARGET_ARCH) -E -Wp,-C,-dD,-dI
%.pp : %.c FORCE
	$(PREPROCESS.c) $< > $@
	
		

