
FRDM_LIBS := /usr/lib/arm-none-eabi/lib

-include sources.mk

CC := gcc
CFLAGS := -Wall -g -O0 -std=c99
LFLAGS := 
RM := rm -rf

OP_EXE_NAME := project

OP_LIB_NAME := libprojec1.a

# No architecture defined, use the host architecture
ifndef ($(ARCH))
	ARCH := $(shell uname -m)
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
	TOOLS_DIR := /usr/bin
	CC := $(TOOLS_DIR)/arm-linux-gnueabihf-gcc
else ifeq ($(ARCH),frdm)
	OBJ_DIR := ./frdm/
	TOOLS_DIR := /usr/bin
	CC := $(TOOLS_DIR)/arm-none-eabi-gcc
	CFLAGS = -mcpu=cortex-m0plus -mthumb -DFRDM
	LFLAGS = -specs=nosys.specs
else
	exit
endif

# Create the output directory if it does not exist
dummy := $(shell test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR))

build: compile-all 
	@echo 'Building $(OP_EXE_NAME) for $(ARCH)'
	$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) -o $(OBJ_DIR)$(OP_EXE_NAME) $(OBJ_DIR)*.o 

preprocess:


asm-file:


%.o: $(SRCS_DIR)/%.c
	@echo 'Building $< for $(ARCH)'
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)$@

compile-all: $(OBJS)


upload:
	scp ./bbb/$(OP_EXE_NAME) root@192.168.1.111:/home/root/Project1/$(OP_EXE_NAME)

clean:
	$(RM) ./x86_64/* ./bbb/* ./frdm/*

build-lib:

