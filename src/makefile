SHELL := cmd.exe

flex := c:\Programme\3\win_flex_bison-latest\win_flex.exe
bison := c:\Programme\3\win_flex_bison-latest\win_bison.exe

protozasm.exe: lex.yy.c protozasm.tab.c ; g++ -o $@ $^
protozasm.tab.c: protozasm.y ; $(bison) -d $<
protozasm.tab.h: protozasm.y ; $(bison) -d $<
lex.yy.c: protozasm.l ; $(flex) --wincompat $<

.PHONY: clean
clean:
	del *.o 2>NUL
	del protozasm.exe 2>NUL
	del protozasm.tab.c 2>NUL
	del protozasm.tab.h 2>NUL
	del lex.yy.c 2>NUL

.PHONY: run
run: protozasm.exe osm.protozasm ; protozasm.exe osm.protozasm
