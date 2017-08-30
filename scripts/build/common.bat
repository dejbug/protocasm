@ECHO off

ECHO -- [ ENTER ] scripts\build\common.bat

IF NOT EXIST appveyor.yml (
	ECHO -- [ ERROR ] this should only be called from root dir.
	GOTO :EOF
)

ECHO -- ensuring proper tree

CALL scripts\build\clean.bat

ECHO -- making new tree
IF NOT EXIST build MKDIR build
IF NOT EXIST test MKDIR test
IF NOT EXIST deploy MKDIR deploy

:LEAVE
ECHO -- [ LEAVE ] scripts\build\common.bat
