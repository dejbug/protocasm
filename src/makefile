SHELL := cmd.exe

flex := c:\Programme\3\win_flex_bison-latest\win_flex.exe
bison := c:\Programme\3\win_flex_bison-latest\win_bison.exe

protozosm.exe: lex.yy.c protozosm.tab.c ; g++ -o $@ $^
protozosm.tab.c: protozosm.y ; $(bison) -d $<
protozosm.tab.h: protozosm.y ; $(bison) -d $<
lex.yy.c: protozosm.l ; $(flex) --wincompat $<

.PHONY: clean
clean:
	del *.o 2>NUL
	del protozosm.exe 2>NUL
	del protozosm.tab.c 2>NUL
	del protozosm.tab.h 2>NUL
	del lex.yy.c 2>NUL

.PHONY: run
run: protozosm.exe script.dejan ; protozosm.exe script.dejan
