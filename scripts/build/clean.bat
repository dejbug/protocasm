@ECHO OFF

ECHO -- [ ENTER ] scripts\build\clean.bat

IF NOT EXIST appveyor.yml (
	ECHO -- [ ERROR ] this should only be called from root dir.
	GOTO :EOF
)

ECHO -- cleaning old tree
IF EXIST build RMDIR /S /Q build
IF EXIST test RMDIR /S /Q test
IF EXIST deploy RMDIR /S /Q deploy

:LEAVE
ECHO -- [ LEAVE ] scripts\build\clean.bat
