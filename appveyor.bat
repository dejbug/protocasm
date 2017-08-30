@ECHO off

ECHO -- [ ENTER ] appveyor.bat

IF NOT EXIST appveyor.yml (
	ECHO -- [ ERROR ] this should only be called from root dir.
	GOTO :EOF
)

rem -- This will build the "src" folder.
rem SET SNIPPETS_BUILD=0

rem -- This will build a project in the "snippets" folder.
SET SNIPPETS_BUILD=1

rem -- This selects the snippet to be built, if SNIPPETS_BUILD==1 .
SET ACTIVE_SNIPPET=google_read

rem -- This will do what it says, after the build is done.
SET RUN_TEST=1

rem -- The following are used for the main build, i.e.
rem -- when SNIPPETS_BUILD==0 .
SET WINFLEXBISON=0
SET KEEPSAMPLE=0

rem -- "PhnomPenh.osm.pbf" is a much smaller file; perfect for testing.
rem SET TEST_PBF_URL=http://download.bbbike.org/osm/bbbike/Darmstadt/Darmstadt.osm.pbf
SET TEST_PBF_URL=http://download.bbbike.org/osm/bbbike/PhnomPenh/PhnomPenh.osm.pbf

rem -- This surrenders control into the "scripts" folder, since
rem -- this BAT here was only for SET-ting downstream config vars .
CALL scripts\build\dispatch.bat

:LEAVE
ECHO -- [ LEAVE ] appveyor.bat
