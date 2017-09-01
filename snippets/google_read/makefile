CXXFLAGS := --std=c++11 -Wall -Wpedantic -O2
CXXFLAGS += -Iprotobuf\include
# CXXFLAGS += -Lprotobuf\lib -lprotobuf-lite
# CXXFLAGS += -Lzlib\lib -lz

QUIET := 1>NUL
NOERR := 2>NUL

main.exe : main.o goo.o
main.exe : fileformat.pb.o osmformat.pb.o
main.exe : protobuf\lib\libprotobuf-lite.a
main.exe : zlib\lib\libz.a
main.exe : ; g++ $(LDFLAGS) -o $@ $^

main.o : main.cpp common.hpp | goo.o ; g++ $(CXXFLAGS) -c $<
goo.o : goo.cpp goo.h common.hpp | pb.o ; g++ $(CXXFLAGS) -c $<

.PHONY: pb.o
pb.o : fileformat.pb.o osmformat.pb.o

fileformat.pb.o : fileformat.pb.cc fileformat.pb.h | protobuf\include
fileformat.pb.o : ; g++ $(CXXFLAGS) -c $<
osmformat.pb.o : osmformat.pb.cc osmformat.pb.h | protobuf\include
osmformat.pb.o : ; g++ $(CXXFLAGS) -c $<

fileformat.pb.cc fileformat.pb.h : fileformat.proto | protobuf\bin\protoc.exe
fileformat.pb.cc fileformat.pb.h : ; protobuf\bin\protoc.exe --cpp_out=. $<
osmformat.pb.cc osmformat.pb.h : osmformat.proto | protobuf\bin\protoc.exe
osmformat.pb.cc osmformat.pb.h : ; protobuf\bin\protoc.exe --cpp_out=. $<

protobuf\include : | protobuf
protobuf\bin\protoc.exe : | protobuf
protobuf\lib\libprotobuf-lite.a : | protobuf
zlib\lib\libz.a : | zlib

protobuf : ; 7z x -y ..\..\custom\protobuf.zip
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
	main.exe ..\..\data\Darmstadt.osm.pbf
