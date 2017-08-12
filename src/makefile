# SHELL := cmd.exe
DEBUG := 1

FLEX := flex
BISON := bison
WINFLAGS := --wincompat

CXXFLAGS := -Wpedantic -Wall -O2 --std=c++11

ifeq ($(DEBUG),1)
CXXFLAGS += -g -D_DEBUG -lkernel32
else
endif

CXX := g++ $(CXXFLAGS)

protocasm.exe: machine.o protocasm.a ; $(CXX) -o $@ $^
machine.o: machine.cpp machine.h protocasm.tab.h ; $(CXX) -c -o $@ $<
protocasm.a: protocasm.lex.o protocasm.tab.o ; ar ru $@ $^
protocasm.lex.o: protocasm.lex.c ; $(CXX) -c -o $@ $<
protocasm.tab.o: protocasm.tab.c ; $(CXX) -c -o $@ $<
protocasm.tab.c protocasm.tab.h: protocasm.y machine.h ; $(BISON) -d $<
protocasm.lex.c: protocasm.l ; $(FLEX) $(WINFLAGS) -o $@ $<

.PHONY: clean
clean:
	del *.o 2>NUL
	del *.a 2>NUL
	del protocasm.exe 2>NUL
	del protocasm.tab.c 2>NUL
	del protocasm.tab.h 2>NUL
	del protocasm.lex.c 2>NUL

.PHONY: run
run: protocasm.exe osm.protocasm ; protocasm.exe osm.protocasm
