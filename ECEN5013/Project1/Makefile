
-include sources.mk

OP_EXE_NAME := project
OP_LIB_NAME := libprojec1.a

CC := gcc
CFLAGS := -Wall -g -O0 -std=c99
LDFLAGS := 
RM := rm -rf

# No architecture defined, use the host architecture
ifndef ($(ARCH))
	ARCH := $(shell uname -m)
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
else ifeq ($(ARCH),$(filter $(ARCH),bbb armv7l))
	OBJ_DIR := ./bbb/
	TOOLS_BASE := /usr/bin/arm-linux-gnueabihf-
	CC := $(TOOLS_BASE)gcc
else ifeq ($(ARCH),frdm)
	OBJ_DIR := ./frdm/
	TOOLS_BASE := /usr/bin/arm-none-eabi-
	CC := $(TOOLS_BASE)gcc
	CFLAGS = -mcpu=cortex-m0plus -mthumb -DFRDM -specs=nosys.specs
else
	exit
endif

# Create the output directory if it does not exist
dummy := $(shell test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR))

build: compile-all 
	@echo 'Building $(OP_EXE_NAME) for $(ARCH)'
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OBJ_DIR)$(OP_EXE_NAME) $(OBJ_DIR)*.o 

preprocess:


asm-file:


%.o: $(SRCS_DIR)/%.c
	@echo 'Building $< for $(ARCH)'
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)$@

compile-all: $(OBJS)


upload:
	scp ./bbb/$(OP_EXE_NAME) debian@$(IP_ADDR):/home/root/Project1/$(OP_EXE_NAME)

clean:
	$(RM) ./x86_64/* ./bbb/* ./frdm/*

build-lib:
	@echo 'Building $(OP_LIB_NAME) for $(ARCH)'
	$(TOOLS_BASE)ar cr $(OBJ_DIR)$(OP_LIB_NAME) $(OBJ_DIR)*.o


