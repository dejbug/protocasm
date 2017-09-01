CXXFLAGS := --std=c++11 -Wall -Wpedantic -O2
# CXXFLAGS += -Lzlib\lib -lz

QUIET := 1>NUL
NOERR := 2>NUL

main.exe : main.o
main.exe : zlib\lib\libz.a
main.exe : ; g++ $(LDFLAGS) -o $@ $^

main.o : main.cpp common.hpp ; g++ $(CXXFLAGS) -c $<
zlib\lib\libz.a : | zlib

zlib : ; 7z x -y ..\..\custom\zlib.zip

.PHONY: clean
clean:
	@DEL *.pb.cc *.pb.h $(NOERR)
	@DEL *.o $(NOERR)
	@DEL main.exe $(NOERR)

.PHONY: reallyclean
clean:
	@IF EXIST protobuf RMDIR /S /Q protobuf
	@IF EXIST zlib RMDIR /S /Q zlib

	@DEL *.pb.cc *.pb.h $(NOERR)
	@DEL *.o $(NOERR)
	@DEL main.exe $(NOERR)

.PHONY: run
run: main.exe
	main.exe
