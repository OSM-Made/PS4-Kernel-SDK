SDK_ROOT = ..
CC = g++
AS = g++

INCLUDE = -I$(SDK_ROOT)/include
CFLAGS = -Os -s -std=gnu++11 -ffunction-sections -fdata-sections \
         -fno-builtin -fno-exceptions -fno-asynchronous-unwind-tables \
         -nostdlib -masm=intel -march=btver2 -m64 -mabi=sysv \
         -mcmodel=small -mstackrealign -D_KERNEL

all: crt0.o crt1.o

crt0.o: crt0.s
	$(AS) $(INCLUDE) -c $< -o $@

crt1.o: crt1.c
	$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

.PHONY: all clean