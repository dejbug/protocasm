
include 'macro/if.inc'

_CRT_WARN = 0
_CRT_ERROR = 1
_CRT_ASSERT = 2
_CRT_ERRCNT = 3
LANG_NEUTRAL = 0x00
SUBLANG_DEFAULT = 0x01

LANGID = (SUBLANG_DEFAULT shl 10) + LANG_NEUTRAL


macro dbg_out text*, arg1, arg2, arg3
{
if arg1 eq
	invoke _CrtDbgReport, _CRT_WARN, 0, 0, 0, text
	add esp, 4*(5+0)
else if arg2 eq
	invoke _CrtDbgReport, _CRT_WARN, 0, 0, 0, text, arg1
	add esp, 4*(5+1)
else if arg3 eq
	invoke _CrtDbgReport, _CRT_WARN, 0, 0, 0, text, arg1, arg2
	add esp, 4*(5+2)
else
	invoke _CrtDbgReport, _CRT_WARN, 0, 0, 0, text, arg1, arg2, arg3
	add esp, 4*(5+3)
end if
}


proc print_error uses ebx

	invoke GetLastError
	mov [_lastErrorCode], eax
	
	invoke FormatMessage, FORMAT_MESSAGE_ALLOCATE_BUFFER + FORMAT_MESSAGE_FROM_SYSTEM + FORMAT_MESSAGE_IGNORE_INSERTS, 0, [_lastErrorCode], LANGID, _msgBuf, 0, 0
	
	mov ebx, [_msgBuf]
	mov byte [ebx+eax-2], 0

	dbg_out _msgErr1, [_lastErrorCode], [_lastErrorCode], dword [_msgBuf]
	invoke LocalFree, dword [_msgBuf]
	
	ret

endp


; uses eax ecx edi
; returns edi = strnlen(buf, limit)
macro strnl buf*, limit=-1
{
	mov ecx, limit
	mov edi, [buf]
	xor eax, eax
	repne scasb
	sub edi, [buf]
	dec edi
}


proc strnlen uses ecx edi, buf, limit

.if dword [limit] = 0
	or ecx, -1
.endif

	mov ecx, [limit]
	mov edi, [buf]
	xor eax, eax
	repne scasb
	sub edi, [buf]
	mov eax, edi
	dec eax
	ret
	
endp


proc print uses ecx edi, buf, buflen

.if [_hTerm] = INVALID_HANDLE_VALUE
	invoke GetStdHandle, STD_OUTPUT_HANDLE
	mov [_hTerm], eax
.endif

.if eax = INVALID_HANDLE_VALUE
	ret
.endif

.if dword [buflen] = 0
; 	; or	ecx, -1
; 	mov ecx, 4096 ; max text length we'll accept
; 	mov edi, [buf]
; 	xor eax, eax
; 	repne scasb
; 	sub edi, [buf]
; 	dec edi
; 	mov [buflen], edi
	stdcall strnlen, [buf], 4096
	mov [buflen], eax
.endif

	; invoke WriteConsole, [_hTerm], buf, buflen, _bytesRead, 0
	invoke WriteConsole, [_hTerm], dword [buf], dword [buflen], _bytesRead, 0

.if eax
	mov eax, [_bytesRead]
.endif

	ret
	
endp

