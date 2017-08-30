PROTOC := c:\Programme\3\protobuf\bin\protoc.exe
CXXFLAGS := -Ic:\Programme\3\protobuf\include -Lc:\Programme\3\protobuf\lib -lprotobuf-lite -lz

main.exe : main.o goo.o
main.exe : fileformat.pb.o osmformat.pb.o
main.exe : c:\Programme\3\protobuf\lib\libprotobuf-lite.a
main.exe : c:\Programme\3\mingw32\i686-w64-mingw32\lib\libz.a
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
	rm -f fileformat.pb.cc fileformat.pb.h
	rm -f osmformat.pb.cc osmformat.pb.h
	rm -f main.o main.exe

.PHONY: run
run: main.exe
	main.exe

