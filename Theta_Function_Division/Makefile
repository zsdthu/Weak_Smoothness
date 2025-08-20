# This Makefile compiles the implementation in this directory along with
# the known answer tests generator located in the
# ../../../KAT/generator/ directory. The output is an executable program
# in the build/ subdirectory, whose name starts with 'kat', followed by
# the implementation name (e.g. 'kat512int' for the 'falcon512int'
# implementation). This program, when executed, generates the .req and
# .rsp files in the expected NIST format.

.POSIX:

CC = clang
CFLAGS = -Wall -Wextra -Wshadow -Wundef -O0 #-pg -fno-pie
LD = clang
LDFLAGS = #-pg -no-pie
LIBS = #-lm

OBJ1 =  build/fpr.o build/rng.o build/shake.o

OBJ3 =  build/speed.o build/cpucycles.o

HEAD1 = api.h fpr.h inner.h cpucycles.h

all: build build/speed

build:
	-mkdir build

clean:
	-rm -f build/*


build/speed: $(OBJ1) $(OBJ3)
	$(LD) $(LDFLAGS) -o build/speed $(OBJ1) $(OBJ3) $(LIBS)

build/fpr.o: fpr.c $(HEAD1)
	$(CC) $(CFLAGS) -c -o build/fpr.o fpr.c

build/rng.o: rng.c $(HEAD1)
	$(CC) $(CFLAGS) -c -o build/rng.o rng.c

build/shake.o: shake.c $(HEAD1)
	$(CC) $(CFLAGS) -c -o build/shake.o shake.c

build/speed.o: speed.c $(HEAD1)
	$(CC) $(CFLAGS) -c -o build/speed.o speed.c

build/cpucycles.o: cpucycles.c $(HEAD2)
	$(CC) $(CFLAGS) -I . -c -o build/cpucycles.o cpucycles.c
