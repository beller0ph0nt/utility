.PHONY: all clear

all: process_b

process_b: process_b.c
	gcc process_b.c -o process_b -fno-stack-protector -g

clear:
	rm -f process_b