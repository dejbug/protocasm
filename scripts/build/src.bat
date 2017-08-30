@ECHO off

ECHO -- [ ENTER ] scripts\build\src.bat

IF NOT EXIST appveyor.yml (
	ECHO -- [ ERROR ] this should only be called from root dir.
	GOTO :EOF
)

ECHO -- [ running ] "stage: prepare"

IF %WINFLEXBISON%==1 (
	ECHO -- installing flex/bison
	cinst winflexbison
)

IF %WINFLEXBISON%==1 (
	ECHO -- renaming winflexbison exes in-place
	PUSHD %WINFLEXBISON_BIN%
	RENAME win_bison.exe bison.exe
	RENAME win_flex.exe flex.exe
	POPD
)

ECHO -- [ ready ] for make

ECHO -- [ running ] "stage: build"

ECHO -- selecting build target

IF %WINFLEXBISON%==1 (
	SET FLEX=C:\ProgramData\chocolatey\lib\winflexbison\tools\win_flex.exe
	SET BISON=C:\ProgramData\chocolatey\lib\winflexbison\tools\win_bison.exe
	SET WINFLAGS=--wincompat
) ELSE (
	SET FLEX=c:\MinGW\msys\1.0\bin\flex.exe
	SET BISON=c:\MinGW\msys\1.0\bin\bison.exe
	SET WINFLAGS=
)

SET MAKEFLAGS=WINFLAGS=%WINFLAGS% FLEX=%FLEX% BISON=%BISON%

IF "%APPVEYOR_REPO_TAG%"=="true" (
	ECHO -- building for release
	SET MAKEFLAGS=%MAKEFLAGS% TARGET=release
)
IF "%APPVEYOR_REPO_TAG%"=="false" (
	ECHO -- building for debug
	SET MAKEFLAGS=%MAKEFLAGS% TARGET=debug
)

PUSHD src
ECHO [ MAKE CMD ] make %MAKEFLAGS%
make %MAKEFLAGS%
POPD

IF NOT EXIST build (
	ECHO -- generating build tree
	MKDIR build
)

ECHO -- moving files
MOVE src\protocasm.exe build\protocasm.exe

ECHO -- patching osm.protocasm
ECHO -- creating patcher for osm.protocasm
PUSHD build
ECHO @ECHO OFF > patcher.bat
ECHO python ..\scripts\patch.py ..\src\osm.protocasm -f -o osm.protocasm ..\data\Darmstadt.osm.pbf ^^>> patcher.bat
python -c "import os; print(os.path.split('%TEST_PBF_URL%')[1])" >> patcher.bat
ECHO -- running patcher for osm.protocasm
CALL patcher.bat
ECHO -- removing patcher
DEL patcher.bat
POPD

ECHO -- writing test script
ECHO CALL fetch_sample.bat > build\run_test.bat
ECHO protocasm.exe osm.protocasm >> build\run_test.bat

ECHO -- running post build scripts
CALL scripts\build\post.bat

:LEAVE
ECHO -- [ LEAVE ] scripts\build\src.bat
