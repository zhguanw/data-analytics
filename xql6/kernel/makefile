KERNEL = fpga_q6

XDEVICE=xilinx:adm-pcie-ku3:2ddr-xpr:3.3
DEVICE_REPO_OPT=

KEEP_TEMP=1
KERNEL_DEBUG=1
CC = g++
CLCC = xocc
CLCC_OPT =  -t ${FLOW} --include ./ -s --xdevice ${XDEVICE} ${DEVICE_REPO_OPT} --xp param:compiler.enableAutoPipelining=false -O0
KERNEL_OBJ = ${KERNEL}.xo

XCLBIN = ${KERNEL}.xclbin

KERNEL_SRCS = ../q6_paged.cpp
KERNEL_INCS = ../q6_paged.h
KERNEL_CLCC_OPT =  -k ${KERNEL}
#KERNEL_CLCC_OPT =   
FLOW=hw
# other options are hw_emu, sw_emu
KERNELS=1
CONSTRAINTFILE= 

XP_VIVADO_PROPS=--xp vivado_prop:run.impl_1.STEPS.OPT_DESIGN.ARGS.DIRECTIVE=NoBramPowerOpt

# verbose messages in place and route
XP_VIVADO_PARAMS =--xp vivado_param:project.writeIntermediateCheckpoints=1
XP_VIVADO_PARAMS+=--xp vivado_param:place.hardVerbose=469538
XP_VIVADO_PARAMS+=--xp vivado_param:place.oldMsgVerbose=1
XP_VIVADO_PARAMS+=--xp vivado_param:route.flowDbg=1
XP_VIVADO_PARAMS+=--xp vivado_param:route.timingDbg=1

CLCC_LINK_OPT= $(XP_VIVADO_PARAMS) $(XP_VIVADO_PROPS)

${XCLBIN}: ${KERNEL_OBJ}
	@echo "************* Compile XCLBIN ${XCLBIN} *************"
	@echo "FLOW = ${FLOW}"
	@echo "KERNELS = ${KERNELS}"
	@echo "Using constraint file $(CONSTRAINTFILE)"
	${CLCC} -l ${CLCC_OPT} ${CLCC_LINK_OPT} $^ -o $@

# sw_emu's gcc seems to want includes in same dir
${KERNEL_OBJ}: ${KERNEL_SRCS} ${KERNEL_INCS}
	cp -f ${KERNEL_INCS} .
	@echo "***** Compiling $@ *****"
	${CLCC} -c ${CLCC_OPT} ${KERNEL_CLCC_OPT} -o $@ ${KERNEL_SRCS}

xbin : ${XCLBIN}
