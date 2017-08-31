@ECHO off

ECHO -- [ ENTER ] scripts\build\common.bat

IF NOT EXIST appveyor.yml (
	ECHO -- [ ERROR ] this should only be called from root dir.
	GOTO :EOF
)

ECHO -- ensuring proper tree

rem CALL scripts\build\clean.bat

ECHO -- making new tree
IF NOT EXIST build MKDIR build
IF NOT EXIST test MKDIR test
IF NOT EXIST deploy MKDIR deploy

ECHO -- setting paths
SET MINGW_BIN=C:\mingw-w64\i686-5.3.0-posix-dwarf-rt_v4-rev0\mingw32\bin
SET WINFLEXBISON_BIN=C:\ProgramData\chocolatey\lib\winflexbison\tools
SET WGET_BIN=c:\MinGW\msys\1.0\bin
SET PATH=%WGET_BIN%;%WINFLEXBISON_BIN%;%MINGW_BIN%;%PATH%

ECHO -- renaming mingw32-make.exe in-place
PUSHD %MINGW_BIN%
RENAME mingw32-make.exe make.exe
POPD

:LEAVE
ECHO -- [ LEAVE ] scripts\build\common.bat
