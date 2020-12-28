NAME = oh_my_cpu
SHELL := $(shell which bash)
g++ := $(shell which g++)
RM := rm -f
TRASH := *.o cpuid_x86 gen.sh
cpuid := lib/cpuid_x86.cpp

.PHONY: all

all: build gen clean

#! Danger, Possible "illegal hardware instruction" error
compile-all: build gen-all clean

build:
	$(g++) -O3 -c $(cpuid)
	$(g++) -O3 -o cpuid_x86 cpuid_x86.o

gen:
	./cpuid_x86 > gen.sh
	$(SHELL) ./gen.sh
	mv a.out $(NAME)

gen-all:
	./cpuid_x86 --compile-all > gen.sh
	$(SHELL) ./gen.sh
	mv a.out $(NAME)

clean:
	$(RM) $(TRASH)

remove: clean
	$(RM) $(NAME)

