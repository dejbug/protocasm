@ECHO off

ECHO -- [ running ] "stage: prepare"

ECHO -- installing flex/bison
cinst winflexbison

ECHO -- setting paths
SET MINGW_BIN=C:\mingw-w64\i686-5.3.0-posix-dwarf-rt_v4-rev0\mingw32\bin
SET WINFLEXBISON_BIN=C:\ProgramData\chocolatey\lib\winflexbison\tools
SET WGET_BIN=c:\MinGW\msys\1.0\bin
SET PATH=%WGET_BIN%;%WINFLEXBISON_BIN%;%MINGW_BIN%;%PATH%

ECHO -- renaming mingw32-make.exe in-place
PUSHD %MINGW_BIN%
RENAME mingw32-make.exe make.exe
POPD

ECHO -- renaming winflexbison exes in-place
PUSHD %WINFLEXBISON_BIN%
RENAME win_bison.exe bison.exe
RENAME win_flex.exe flex.exe
POPD

ECHO -- ensuring proper tree
IF NOT EXIST build MKDIR build
IF NOT EXIST test MKDIR test
IF NOT EXIST deploy MKDIR deploy

ECHO -- [ ready ] for make

ECHO -- [ running ] "stage: build"

ECHO -- selecting build target

IF "%APPVEYOR_REPO_TAG%"=="true" GOTO build_release
IF "%APPVEYOR_REPO_TAG%"=="false" GOTO build_debug
GOTO :EOF

REM REM REM REM REM REM REM REM REM REM REM REM REM REM REM REM REM REM

:build_debug

ECHO -- building for debug

make TARGET=debug WINFLAGS=--wincompat

GOTO run_test

REM REM REM REM REM REM REM REM REM REM REM REM REM REM REM REM REM REM

:build_release

ECHO -- building for release

make TARGET=release WINFLAGS=--wincompat

ECHO %APPVEYOR_PROJECT_NAME%-%APPVEYOR_REPO_TAG_NAME%.zip > build\release.txt

ECHO -- packing output to deploy
7z a deploy\%APPVEYOR_PROJECT_NAME%-%APPVEYOR_REPO_TAG_NAME%.zip build\*

DIR deploy

GOTO run_test

:run_test

ECHO -- running test
PUSHD build
CALL run_test.bat
POPD

ECHO -- [WARN] keeping sample file in build folder
REM ECHO -- deleting sample file in build folder
REM DEL build\*.osm.pbf 2>NUL

GOTO :EOF