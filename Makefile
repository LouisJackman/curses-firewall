CC=cc
CC_FLAGS=-ansi -Wall -Wextra -std=c11

curses-firewall: main.c src/*
	$(CC) $(CC_FLAGS) -o curses-firewall main.c src/*.c -lncurses
