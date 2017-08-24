# SHELL := cmd.exe
CXXFLAGS := -Wpedantic -Wall -O2 --std=c++11
# CXXFLAGS += -D_DEBUG -lkernel32
CXX := g++ $(CXXFLAGS)

simple_read.exe: simple_read.o operations.o raii.o ; $(CXX) -o $@ $^
simple_read.o: simple_read.cpp io.hpp ; $(CXX) -c $<
operations.o: operations.cpp operations.h ; $(CXX) -c $<
raii.o: raii.cpp raii.h ; $(CXX) -c $<
io.hpp: common.hpp

.PHONY: clean
clean:
	del *.o 2>NUL
	del *.exe 2>NUL

.PHONY: run
run: simple_read.exe
	simple_read.exe
