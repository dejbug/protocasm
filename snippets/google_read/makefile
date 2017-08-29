PROTOC := c:\Programme\3\protobuf\bin\protoc.exe
CXXFLAGS := -Ic:\Programme\3\protobuf\include -Lc:\Programme\3\protobuf\lib -lprotobuf

main.exe: main.o fileformat.pb.o c:\Programme\3\protobuf\lib\libprotobuf.a
	g++ $(LDFLAGS) -o $@ $^ 

main.o: main.cpp fileformat.pb.h ; g++ $(CXXFLAGS) -c $<
fileformat.pb.o : fileformat.pb.cc fileformat.pb.h ; g++ $(CXXFLAGS) -c $<

fileformat.pb.cc fileformat.pb.h: fileformat.proto ; $(PROTOC) --cpp_out=. $<
osmformat.pb.cc osmformat.pb.h: osmformat.proto ; $(PROTOC) --cpp_out=. $<

.PHONY: clean
clean:
	rm -f fileformat.pb.cc fileformat.pb.h
	rm -f osmformat.pb.cc osmformat.pb.h
	rm -f main.o main.exe
