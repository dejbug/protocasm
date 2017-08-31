PROTOC := ..\..\extern\protobuf\bin\protoc.exe
CXXFLAGS := --std=c++11 -Wall -Wpedantic -O2
CXXFLAGS += -I..\..\extern\protobuf\include
# CXXFLAGS += -L..\..\extern\protobuf\lib
# CXXFLAGS += -lprotobuf-lite

main.exe : main.o goo.o
main.exe : fileformat.pb.o osmformat.pb.o
main.exe : ..\..\extern\protobuf\lib\libprotobuf-lite.a
main.exe : ..\..\extern\i686-w64-mingw32\lib\libz.a
main.exe : ; g++ $(LDFLAGS) -o $@ $^

main.o : main.cpp common.hpp | goo.o ; g++ $(CXXFLAGS) -c $<
goo.o : goo.cpp goo.h common.hpp | pb.o ; g++ $(CXXFLAGS) -c $<

.PHONY: pb.o
pb.o : fileformat.pb.o osmformat.pb.o

fileformat.pb.o : fileformat.pb.cc fileformat.pb.h ; g++ $(CXXFLAGS) -c $<
osmformat.pb.o : osmformat.pb.cc osmformat.pb.h ; g++ $(CXXFLAGS) -c $<

fileformat.pb.cc fileformat.pb.h : fileformat.proto ; $(PROTOC) --cpp_out=. $<
osmformat.pb.cc osmformat.pb.h : osmformat.proto ; $(PROTOC) --cpp_out=. $<

.PHONY: clean
clean:
	rm -f *.pb.cc *.pb.h
	rm -f *.o
	rm -f main.exe

.PHONY: run
run: main.exe
	main.exe "..\\..\\data\\Darmstadt.osm.pbf"
