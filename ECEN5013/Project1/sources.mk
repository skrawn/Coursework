SRCS += main.c \
	data.c \
	memory.c \
	project_1.c

SRCS_DIR = ./src

OBJS = $(SRCS:.c=.o)

ASMS = $(SRCS:.c=.s)

PREPROC = $(SRCS:.c=.i)

INCLUDES = -I./inc
