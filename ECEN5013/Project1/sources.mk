SRCS += main.c \
	data.c \
	memory.c \
	project_1.c

SRCS_DIR = ./src

OBJS = $(SRCS:.c=.o)

INCLUDES = -I./inc
