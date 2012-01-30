DESIGN          = vga_top
PINS            = $(DESIGN).ucf
DEVICE          = xc3s500e-VQ100-4
BGFLAGS         = -g TdoPin:PULLNONE -g DonePin:PULLUP \
                  -g CRC:enable -g StartUpClk:CCLK

SIM_CMD         = /opt/cad/modeltech/bin/vsim 
SIM_COMP_SCRIPT = simulation/$(DESIGN)_TB.do
#SIM_INIT_SCRIPT = simulation/$(DESIGN)_init.do
SIMGEN_OPTIONS  = -p $(FPGA_ARCH) -lang $(LANGUAGE)
SAKC_IP         = 192.168.1.2

SRC             = $(DESIGN).v 
SRC_HDL    		= vga_controller_640_60.vhd
 
all:            bits

remake:         clean-build all

clean:
	rm -f *~ */*~ a.out *.log *.key *.edf *.ps trace.dat

clean-build: clean
	rm -rf build

cleanall:   clean
	rm -rf build $(DESIGN).bit

bits:           $(DESIGN).bit

#
# Synthesis
#
build/project.src:
	@[ -d build ] || mkdir build
	@rm -f $@
	for i in $(SRC); do echo verilog work ../$$i >> $@; done
	for i in $(SRC_HDL); do echo VHDL work ../$$i >> $@; done

build/project.xst: build/project.src
	echo "run" > $@
	echo "-top $(DESIGN) " >> $@
	echo "-p $(DEVICE)" >> $@
	echo "-opt_mode Area" >> $@
	echo "-opt_level 1" >> $@
	echo "-ifn project.src" >> $@
	echo "-ifmt mixed" >> $@
	echo "-ofn project.ngc" >> $@
	echo "-ofmt NGC" >> $@
	echo "-rtlview yes" >> $@

build/project.ngc: build/project.xst $(SRC)
	cd build && xst -ifn project.xst -ofn project.log

build/project.ngd: build/project.ngc $(PINS)
	cd build && ngdbuild -p $(DEVICE) project.ngc -uc ../$(PINS)

build/project.ncd: build/project.ngd
	cd build && map -pr b -p $(DEVICE) project

build/project_r.ncd: build/project.ncd
	cd build && par -w project project_r.ncd

build/project_r.twr: build/project_r.ncd
	cd build && trce -v 25 project_r.ncd project.pcf

$(DESIGN).bit:  build/project_r.ncd build/project_r.twr
	cd build && bitgen project_r.ncd -l -w $(BGFLAGS)
	@mv -f build/project_r.bit $@
sim: 
	cd simulation; $(SIM_CMD) -do $(DESIGN)_TB.do 

upload: $(DESIGN).bit
	scp $(DESIGN).bit root@$(SAKC_IP): 
