SET WINFLEXBISON=0

ECHO -- [ running ] "stage: prepare"

ECHO -- installing flex/bison
IF WINFLEXBISON==1 (
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

IF WINFLEXBISON==1 (
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

IF WINFLEXBISON==1 (
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
cmd /c make %MAKEFLAGS%
POPD

ECHO -- generating build tree
IF NOT EXIST build MKDIR build

ECHO -- moving files
MOVE src\protocasm.exe build\protocasm.exe
python src\patch.py src\osm.protocasm -f -o build\osm.protocasm -b Ii4uXGRhdGFcRGFybXN0YWR0Lm9zbS5wYmYi IlBobm9tUGVuaC5vc20ucGJmIg==

ECHO -- writing scripts
ECHO IF NOT EXIST %%~dp0\$(notdir $(TEST_PBF_URL)) wget -P %%~dp0 --no-check-certificate $(TEST_PBF_URL) > build\fetch_sample.bat
ECHO CALL %%~dp0\fetch_sample.bat > build\run_test.bat
ECHO %%~dp0\protocasm.exe %%~dp0\osm.protocasm >> build\run_test.bat

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

ECHO -- [WARN] keeping sample file in build folder
REM ECHO -- deleting sample file in build folder
REM DEL build\*.osm.pbf 2>NUL
