
HOST_TOOLS_DIR :=
BBB_TOOLS_DIR := /usr/bin/arm-linux-gnueabihf-
FRDM_TOOLS_DIR := /usr/bin/arm-none-eabi-

HOST_OBJ_DIR := ./host/
BBB_OBJ_DIR := ./bbb/
FRDM_OBJ_DIR := ./frdm/

FRDM_LIBS := /usr/lib/arm-none-eabi/lib

-include sources.mk

CC := gcc
CFLAGS := -Wall -g -O0
RM := rm -rf

OP_EXE_NAME := project

OP_LIB_NAME := libprojec1.a

build: compile-all 
	@echo 'Building for host'
	$(CC) $(CFLAGS) $(INCLUDES) -o $(HOST_OBJ_DIR)$(OP_EXE_NAME) $(HOST_OBJ_DIR)*.o 
	@echo 'Building for BeagleBone Black'
	$(BBB_TOOLS_DIR)$(CC) $(CFLAGS) $(INCLUDES) -o $(BBB_OBJ_DIR)$(OP_EXE_NAME) $(BBB_OBJ_DIR)*.o
	@echo 'Building for FRDM'
	$(FRDM_TOOLS_DIR)$(CC) $(CFLAGS) $(INCLUDES) -o $(FRDM_OBJ_DIR)$(OP_EXE_NAME) $(FRDM_OBJ_DIR)*.o -L$(FRDM_LIBS) -lc

preprocess:


asm-file:


%.o: $(SRCS_DIR)/%.c
	@echo 'Building $<'
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(HOST_OBJ_DIR)$@
	$(BBB_TOOLS_DIR)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(BBB_OBJ_DIR)$@
	$(FRDM_TOOLS_DIR)$(CC) $(CFLAGS) $(INCLUDES) -DFRDM -c $< -o $(FRDM_OBJ_DIR)$@

compile-all: $(OBJS)

bbb:
	ARCH := bbb

upload:


clean:
	$(RM) $(HOST_OBJ_DIR)* $(BBB_OBJ_DIR)* $(FRDM_OBJ_DIR)*

build-lib:

