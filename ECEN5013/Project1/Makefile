

OP_EXE_NAME := project
OP_LIB_NAME := libprojec1.a

CC := gcc
CFLAGS += -Wall -g -O0 -std=c99
LDFLAGS := 
RM := rm -rf

# No architecture defined, use the host architecture
ifndef ($(ARCH))
	ARCH := $(shell uname -m)
endif

ifeq ($(VERBOSE), 1)
	CFLAGS += -DVERBOSE
endif

ifeq ($(UNIT_TEST), 1)
	CFLAGS += -DUNIT_TEST
endif

# No IP address was given for upload. Use default
ifndef ($(IP_ADDR))
	IP_ADDR := 192.168.1.111
endif

ifeq ($(ARCH),$(filter $(ARCH),x86_64 host))
	# host could apply to either the x86_64 host machine or the BBB
	ifeq ($(shell uname -m),x86_64)
		OBJ_DIR := ./x86_64/
	else ifeq ($(shell uname -m),armv7l)
		OBJ_DIR := ./bbb/
	endif
	CFLAGS += -D_XOPEN_SOURCE=700
	LDFLAGS += -lrt	-lc
	INCLUDES += -I/usr/include
else ifeq ($(ARCH),$(filter $(ARCH),bbb armv7l))
	OBJ_DIR := ./bbb/
	TOOLS_BASE := /usr/bin/arm-linux-gnueabihf-
	CC := $(TOOLS_BASE)gcc
	CFLAGS += -D_XOPEN_SOURCE=700
	LDFLAGS += -lrt	-lc
	INCLUDES += -I/usr/include
else ifeq ($(ARCH),frdm)
	OBJ_DIR := ./frdm/
	TOOLS_BASE := /usr/bin/arm-none-eabi-
	CC := $(TOOLS_BASE)gcc
	CFLAGS += -mcpu=cortex-m0plus -mthumb -DFRDM -D"CPU_MKL25Z4128VFM4" -specs=nosys.specs
	KINETIS_SDK = $(HOME)/Downloads/SDK_2.0_FRDM-KL25Z
	INCLUDES += -I$(KINETIS_SDK)/CMSIS/Include \
		-I$(KINETIS_SDK)/devices/MKL25Z4
	LDFLAGS += -T "$(KINETIS_SDK)/devices/MKL25Z4/gcc/MKL25Z128xxx4_flash.ld"
	# Call the special rule to create the startup object file
	OBJS += startup
else
	exit
endif

-include sources.mk

# Create the output directory if it does not exist
dummy := $(shell test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR))

build: compile-all 
	@echo 'Building $(OP_EXE_NAME) for $(ARCH)'
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OBJ_DIR)$(OP_EXE_NAME) $(OBJ_DIR)*.o -Wl,-Map=$(OBJ_DIR)$(OP_EXE_NAME).map -lm $(LDFLAGS)
	$(TOOLS_BASE)size $(OBJ_DIR)$(OP_EXE_NAME)
	$(TOOLS_BASE)objdump -S $(OBJ_DIR)$(OP_EXE_NAME) > $(OBJ_DIR)$(OP_EXE_NAME).objdump

preprocess: $(PREPROC)

%.i: $(SRCS_DIR)/%.c
	@echo 'Generating $< for $(ARCH)'
	$(CC) $(CFLAGS) $(INCLUDES) -E $< -o $(OBJ_DIR)$@

asm-file: $(ASMS)
	
%.s: $(SRCS_DIR)/%.c
	@echo 'Generating $< for $(ARCH)'
	$(CC) $(CFLAGS) $(INCLUDES) -S $< -o $(OBJ_DIR)$@

%.o: $(SRCS_DIR)/%.c
	@echo 'Building $< for $(ARCH)'
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)$@

startup:
	$(CC) $(CFLAGS) $(INCLUDES) -x assembler-with-cpp -D__NO_SYSTEM_INIT -c $(KINETIS_SDK)/devices/MKL25Z4/gcc/startup_MKL25Z4.S -o $(OBJ_DIR)startup_MKL25Z4.o

compile-all: $(OBJS)

upload:
	scp ./bbb/$(OP_EXE_NAME) debian@$(IP_ADDR):/home/root/Project1/$(OP_EXE_NAME)

clean:
	$(RM) ./x86_64/* ./bbb/* ./frdm/*

build-lib:
	@echo 'Building $(OP_LIB_NAME) for $(ARCH)'
	$(TOOLS_BASE)ar cr $(OBJ_DIR)$(OP_LIB_NAME) $(OBJ_DIR)*.o

printvar:
	@echo 'SRCS = $(SRCS)'
	@echo 'SRCS_DIR = $(SRCS_DIR)'
	@echo 'OBJS = $(OBJS)'

.PHONY: build preprocess asm-file compile-all upload clean build-lib
