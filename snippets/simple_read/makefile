# SHELL := cmd.exe
CXXFLAGS := -Wpedantic -Wall -O2 --std=c++11
# CXXFLAGS += -D_DEBUG -lkernel32
CXX := g++ $(CXXFLAGS)

simple_read.exe: simple_read.cpp ; $(CXX) -o $@ $^

.PHONY: clean
clean:
	del *.o 2>NUL
	del *.exe 2>NUL

.PHONY: run
run: simple_read.exe
	simple_read.exe
