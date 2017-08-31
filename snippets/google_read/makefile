PROTOC := protobuf\bin\protoc.exe
CXXFLAGS := --std=c++11 -Wall -Wpedantic -O2
CXXFLAGS += -Iprotobuf\include
# CXXFLAGS += -Lprotobuf\lib -lprotobuf-lite
# CXXFLAGS += -Lzlib\lib -lz

main.exe : main.o goo.o
main.exe : fileformat.pb.o osmformat.pb.o
main.exe : protobuf\lib\libprotobuf-lite.a
main.exe : zlib\lib\libz.a
main.exe : ; g++ $(LDFLAGS) -o $@ $^

.PHONY: custom
custom:
	@IF NOT EXIST protobuf 7z x -y ..\..\custom\protobuf.zip
	@IF NOT EXIST zlib 7z x -y ..\..\custom\zlib.zip

main.o : main.cpp common.hpp | goo.o ; g++ $(CXXFLAGS) -c $<
goo.o : goo.cpp goo.h common.hpp | pb.o ; g++ $(CXXFLAGS) -c $<

.PHONY: pb.o
pb.o : fileformat.pb.o osmformat.pb.o

fileformat.pb.o : fileformat.pb.cc fileformat.pb.h ; g++ $(CXXFLAGS) -c $<
osmformat.pb.o : osmformat.pb.cc osmformat.pb.h ; g++ $(CXXFLAGS) -c $<

fileformat.pb.cc fileformat.pb.h : fileformat.proto | custom ; $(PROTOC) --cpp_out=. $<
osmformat.pb.cc osmformat.pb.h : osmformat.proto | custom ; $(PROTOC) --cpp_out=. $<

.PHONY: clean
clean:
	@DEL *.pb.cc *.pb.h 2>NUL
	@DEL *.o 2>NUL
	@DEL main.exe 2>NUL
	@IF EXIST protobuf RMDIR /S /Q protobuf 2>NUL
	@IF EXIST zlib RMDIR /S /Q zlib

.PHONY: run
run: main.exe
	main.exe "..\\..\\data\\Darmstadt.osm.pbf"
