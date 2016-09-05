
-include sources.mk

HOST_TOOLS_DIR :=
BBB_TOOLS_DIR := /usr/arm-linux-gnueabi/bin/
FRDM_TOOLS_DIR := /usr/arm-none-eabi/bin/

CC := gcc
CFLAGS := -Wall -g -O0
RM := rm -rf

OP_EXE_NAME := project

OP_LIB_NAME := libprojec1.a

build: compile-all 
	@echo 'Building for host'
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OP_EXE_NAME) $(OBJS)
    

preprocess:


asm-file:


%.o: $(SRCS_DIR)/%.c
	@echo 'Building $<'
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 	

compile-all: $(OBJS)


upload:


clean:
	$(RM) *.o

build-lib:

