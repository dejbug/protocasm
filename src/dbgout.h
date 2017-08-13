#ifndef _DBGOUT_H_
#define _DBGOUT_H_

#ifdef _DEBUG
// Don't forget to CXXFLAGS += -lkernel32 !
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA (char const *);
#endif

template<size_t N=1024>
void dbgout(char const * format, ...)
{
#ifdef _DEBUG
	char buffer[N + 1] = {0};

	va_list args;
	va_start(args, format);
	__mingw_vsnprintf(buffer, N, format, args);
	va_end(args);

	// _RPT0(_CRT_WARN, buffer);
	OutputDebugStringA(buffer);
#endif
}

#endif // _DBGOUT_H_
