@ECHO off

SET WINFLEXBISON=0
SET KEEPSAMPLE=1
SET TEST_PBF_URL=http://download.bbbike.org/osm/bbbike/PhnomPenh/PhnomPenh.osm.pbf

ECHO -- [ running ] "stage: prepare"

IF %WINFLEXBISON%==1 (
	ECHO -- installing flex/bison
	cinst winflexbison
)

ECHO -- setting paths
SET MINGW_BIN=C:\mingw-w64\i686-5.3.0-posix-dwarf-rt_v4-rev0\mingw32\bin
SET WINFLEXBISON_BIN=C:\ProgramData\chocolatey\lib\winflexbison\tools
SET WGET_BIN=c:\MinGW\msys\1.0\bin
SET PATH=%WGET_BIN%;%WINFLEXBISON_BIN%;%MINGW_BIN%;%PATH%

ECHO -- renaming mingw32-make.exe in-place
PUSHD %MINGW_BIN%
RENAME mingw32-make.exe make.exe
POPD

IF %WINFLEXBISON%==1 (
	ECHO -- renaming winflexbison exes in-place
	PUSHD %WINFLEXBISON_BIN%
	RENAME win_bison.exe bison.exe
	RENAME win_flex.exe flex.exe
	POPD
)

ECHO -- ensuring proper tree
IF NOT EXIST build MKDIR build
IF NOT EXIST test MKDIR test
IF NOT EXIST deploy MKDIR deploy

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
python src\patch.py src\osm.protocasm -f -o build\osm.protocasm -b Ii4uXGRhdGFcRGFybXN0YWR0Lm9zbS5wYmYi IlBobm9tUGVuaC5vc20ucGJmIg==

ECHO -- writing scripts
python -c "import os; print('IF NOT EXIST \x22' + os.path.split('%TEST_PBF_URL%')[1] + '\x22 wget --no-check-certificate %TEST_PBF_URL%')" > build\fetch_sample.bat
ECHO CALL fetch_sample.bat > build\run_test.bat
ECHO protocasm.exe osm.protocasm >> build\run_test.bat

ECHO -- writing info
ECHO [InternetShortcut] > build\protocasm.url
ECHO URL=https://github.com/dejbug/protocasm >> build\protocasm.url

IF "%APPVEYOR_REPO_TAG%"=="true" (
	ECHO -- packing output to deploy
	ECHO %APPVEYOR_PROJECT_NAME%-%APPVEYOR_REPO_TAG_NAME%.zip > build\release.txt
	7z a deploy\%APPVEYOR_PROJECT_NAME%-%APPVEYOR_REPO_TAG_NAME%.zip build\*
	DIR deploy
)

ECHO -- running test
PUSHD build
CALL run_test.bat
POPD

IF %KEEPSAMPLE%==1 (
	ECHO -- [ FLAG ] keeping sample file in build folder
) ELSE (
	ECHO -- deleting sample file in build folder
	DEL build\*.osm.pbf 2>NUL
)
