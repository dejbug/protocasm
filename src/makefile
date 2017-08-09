SHELL := cmd.exe
CXXFLAGS := -Wpedantic -Wall -O2 --std=c++11

protocasm.exe: protocasm.lex.c protocasm.tab.c ; g++ $(CXXFLAGS) -o $@ $^
protocasm.tab.c protocasm.tab.h: protocasm.y machine.h ; bison -d $<
protocasm.lex.c: protocasm.l ; flex --wincompat -o $@ $<


.PHONY: clean
clean:
	del *.o 2>NUL
	del protocasm.exe 2>NUL
	del protocasm.tab.c 2>NUL
	del protocasm.tab.h 2>NUL
	del protocasm.lex.c 2>NUL

.PHONY: run
run: protocasm.exe osm.protocasm ; protocasm.exe osm.protocasm
