
main.exe: main.asm ; fasm $<
main.exe: debug.inc

# main.exe: main.obj debug.obj console.obj; ld -o $@ $^
# main.obj: main.asm ; fasm $<
# debug.obj: debug.asm ; fasm $<
# console.obj: console.asm ; fasm $<

.PHONY: run
run: main.exe
	main.exe
