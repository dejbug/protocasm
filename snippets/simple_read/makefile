# SHELL := cmd.exe
CXXFLAGS := -Wpedantic -Wall -O2 --std=c++11
# CXXFLAGS += -D_DEBUG -lkernel32
CXX := g++ $(CXXFLAGS)

simple_read.exe: simple_read.o pb.o osm.o ; $(CXX) -o $@ $^
simple_read.o: simple_read.cpp common.hpp ; $(CXX) -c $<
pb.o: pb.cpp pb.h common.hpp ; $(CXX) -c $<
osm.o: osm.cpp osm.h pb.h common.hpp ; $(CXX) -c $<

.PHONY: clean
clean:
	del *.o 2>NUL
	del *.exe 2>NUL

.PHONY: run
run: simple_read.exe
	simple_read.exe
