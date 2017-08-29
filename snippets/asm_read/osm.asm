
; osm.asm - Yet another *.osm.pbf files parser.
; <https://github.com/dejbug>

; format PE GUI 4.0
format PE console 4.0

include 'win32a.inc'
include 'debug.inc'

ERROR_HANDLE_EOF = 38

LANG_NEUTRAL = 0x00
SUBLANG_DEFAULT = 0x01

LANGID = (SUBLANG_DEFAULT shl 10) + LANG_NEUTRAL

; no section defined - fasm will automatically create .flat section for both
; code and data, and set entry point at the beginning of this section

	invoke GetStdHandle, STD_OUTPUT_HANDLE
	cmp eax, INVALID_HANDLE_VALUE
	je exit
	mov [_hTerm], eax
	
	invoke WriteConsole, [_hTerm], _filePath, 25, _bytesRead, 0
	cmp eax, 0
	je unk_error
	invoke WriteConsole, [_hTerm], _endl, 2, _bytesRead, 0
	
	; invoke con_out, _filePath, 25

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
	dbg_out _szMsg5, [_bytesRead]
	dbg_out _filePath

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


proc print_error uses ebx

	invoke GetLastError
	mov [_lastErrorCode], 6
	
	invoke FormatMessage, FORMAT_MESSAGE_ALLOCATE_BUFFER + FORMAT_MESSAGE_FROM_SYSTEM + FORMAT_MESSAGE_IGNORE_INSERTS, 0, [_lastErrorCode], LANGID, _msgBuf, 0, 0
	
	mov ebx, [_msgBuf]
	mov byte [ebx+eax-2], 0

	invoke WriteConsole, [_hTerm], dword [_msgBuf], eax, _bytesRead, 0
	dbg_out _msgErr1, [_lastErrorCode], [_lastErrorCode], 0
	; dbg_out _msgErr1, 0,0,dword [_msgBuf]
	; dbg_out _msgErr2, [_lastErrorCode], eax
	invoke LocalFree, dword [_msgBuf]
	
	ret

endp

_lastErrorCode dd 0
_msgBuf dd 0
_msgErr1 db '! error %08X (%d) "%s"', 0
_msgErr2 db '! error %08X (%d)', 0

_filePath db '..\..\data\Darmstadt.osm.pbf', 0
_szMsg1 db '* opening "%s"', 0
_szMsg2 db '* closing "%s"', 0
_szMsg3 db '* [HANDLE = %08x] : "%s"', 0
_szMsg4 db '! could not read from file "%s"', 0
_szMsg5 db '* %d bytes read from file', 0
_szMsg6 db '! could not open file "%s"', 0
_endl db 0xD, 0xA, 0
_hTerm dd INVALID_HANDLE_VALUE
_hFile dd INVALID_HANDLE_VALUE
_buffer rb 512
_bytesRead dd 0

; import data in the same section

data import

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

end data
