@ECHO off

ECHO -- [ ENTER ] scripts\build\google_read.bat

IF NOT EXIST appveyor.yml (
	ECHO -- [ ERROR ] this should only be called from root dir.
	GOTO :EOF
)

PUSHD snippets\google_read
make
IF ERRORLEVEL 1 (
	ECHO -- [ ERROR ] make failed: rest of script makes no sense.
	goto LEAVE
)
POPD

ECHO -- moving files
MOVE snippets\google_read\main.exe build\google_read.exe

ECHO -- writing test script
ECHO CALL fetch_sample.bat > build\run_test.bat
ECHO google_read.exe *.osm.pbf >> build\run_test.bat

ECHO -- running post build scripts
CALL scripts\build\post.bat

:LEAVE
ECHO -- [ LEAVE ] scripts\build\google_read.bat
