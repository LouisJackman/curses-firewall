.POSIX:

CC=cc
CC_FLAGS=-ansi -Wall -Wextra -std=c11

all: main.c src/*
	$(CC) $(CC_FLAGS) -o curses-firewall main.c src/*.c -lncurses
