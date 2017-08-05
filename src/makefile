SHELL := cmd.exe

protocasm.exe: lex.yy.c protocasm.tab.c ; g++ -o $@ $^
protocasm.tab.c: protocasm.y ; bison -d $<
protocasm.tab.h: protocasm.y ; bison -d $<
lex.yy.c: protocasm.l ; flex --wincompat $<

.PHONY: clean
clean:
	del *.o 2>NUL
	del protocasm.exe 2>NUL
	del protocasm.tab.c 2>NUL
	del protocasm.tab.h 2>NUL
	del lex.yy.c 2>NUL

.PHONY: run
run: protocasm.exe osm.protocasm ; protocasm.exe osm.protocasm
