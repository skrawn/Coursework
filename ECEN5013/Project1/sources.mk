SRCS += main.c \
	data.c \
	memory.c \
	project_1.c \
	uart.c \
	circ_buf.c \
	proc_init.c \
	log.c \
	rgb_led.c \
	profiler.c \
	project_2.c \
	unit_test.c \
	spi.c \
	nRF24L01.c \
	dma.c \
	project_3.c

SRCS_DIR += ./src

OBJS += $(SRCS:.c=.o)

ASMS += $(SRCS:.c=.s)

PREPROC += $(SRCS:.c=.i)

INCLUDES += -I./inc
