@ECHO off

ECHO -- [ ENTER ] scripts\build\dispatch.bat

IF NOT EXIST appveyor.yml (
	ECHO -- [ ERROR ] this should only be called from root dir.
	GOTO :EOF
)

ECHO -- running common setup script
CALL scripts\build\common.bat

IF %SNIPPETS_BUILD%==0 (
	CALL scripts\build\src.bat
	GOTO LEAVE
) ELSE IF NOT %ACTIVE_SNIPPET%=="" (
	IF NOT EXIST scripts\build\%ACTIVE_SNIPPET%.bat (
		ECHO [ ERROR ] invalid snippet id '%ACTIVE_SNIPPET%'
		ECHO [   ... ] "scripts\build\%ACTIVE_SNIPPET%.bat" not found
		GOTO LEAVE
	)
	CALL scripts\build\%ACTIVE_SNIPPET%.bat
	GOTO LEAVE
)

:LEAVE
ECHO -- [ LEAVE ] scripts\build\dispatch.bat
