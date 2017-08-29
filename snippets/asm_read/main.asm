
; osm/main.asm - Yet another *.osm.pbf files parser.
; <https://github.com/dejbug>

; format PE GUI 4.0
format PE console 4.0
entry start


include 'win32a.inc'
include 'debug.inc'

ERROR_HANDLE_EOF = 38

section '.text' code readable executable

start:
	stdcall print, _filePath, 0
	stdcall print, _endl, 2
	dbg_out _szMsg1, _filePath

	invoke CreateFile, _filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0
	cmp eax, INVALID_HANDLE_VALUE
	je fnf_error
	mov [_hFile], eax
	dbg_out _szMsg3, [_hFile], _filePath

	invoke ReadFile, [_hFile], _buffer, 512, _bytesRead, 0
	cmp eax, 0
	je read_error
	cmp [_bytesRead], 0
	je eof_error
	dbg_out _szMsg5, [_bytesRead], _filePath
	
	; ...

	jmp close_and_exit

unk_error:
	call print_error
	jmp exit

fnf_error:
	dbg_out _szMsg6, _filePath
	jmp exit

eof_error:
	jmp close_and_exit

read_error:
	dbg_out _szMsg4, _filePath

close_and_exit:
	dbg_out _szMsg2, _filePath
	invoke CloseHandle, [_hFile]

exit:
	invoke ExitProcess, 0


section '.data' data readable writeable

_hTerm dd INVALID_HANDLE_VALUE
_hFile dd INVALID_HANDLE_VALUE
_lastErrorCode dd 0
_msgBuf dd 0
_bytesRead dd 0

_filePath db '..\..\data\Darmstadt.osm.pbf', 0

_endl db 0xD, 0xA, 0

_msgErr1 db '! error %08X (%d) "%s"', 0
_msgErr2 db '! error %08X (%d)', 0

_szMsg1 db '* opening "%s"', 0
_szMsg2 db '* closing "%s"', 0
_szMsg3 db '* [HANDLE = %08x] : "%s"', 0
_szMsg4 db '! could not read from file "%s"', 0
_szMsg5 db '* %d bytes read from file "%s"', 0
_szMsg6 db '! could not open file "%s"', 0


section '.bss' data readable writeable

_buffer rb 512


section '.idata' import data readable writeable

library \
	kernel32, 'KERNEL32.DLL', \
	msvcrtd, 'MSVCRTD.dll'

import kernel32, \
	ExitProcess, 'ExitProcess', \
	GetLastError, 'GetLastError', \
	FormatMessage, 'FormatMessageA', \
	GetStdHandle, 'GetStdHandle', \
	WriteConsole, 'WriteConsoleA', \
	CreateFile, 'CreateFileA', \
	CloseHandle, 'CloseHandle', \
	LocalFree, 'LocalFree', \
	ReadFile, 'ReadFile'

import msvcrtd, \
	_CrtDbgReport, '_CrtDbgReport'


; section '.edata' export data readable

section '.reloc' fixups data readable discardable

  if $=$$
    dd 0,8		; if there are no fixups, generate dummy entry
  end if
