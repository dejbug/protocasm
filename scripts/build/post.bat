@ECHO off

ECHO -- [ ENTER ] scripts\build\post.bat

IF NOT EXIST appveyor.yml (
	ECHO -- [ ERROR ] this should only be called from root dir.
	GOTO :EOF
)

ECHO -- writing fetch script
python -c "import os; print('IF NOT EXIST \x22' + os.path.split('%TEST_PBF_URL%')[1] + '\x22 wget --no-check-certificate %TEST_PBF_URL%')" > build\fetch_sample.bat

ECHO -- writing info
ECHO [InternetShortcut] > build\protocasm.url
ECHO URL=https://github.com/dejbug/protocasm >> build\protocasm.url

IF "%APPVEYOR_REPO_TAG%"=="true" (
	ECHO -- packing output to deploy
	ECHO %APPVEYOR_PROJECT_NAME%-%APPVEYOR_REPO_TAG_NAME%.zip > build\release.txt
	7z a deploy\%APPVEYOR_PROJECT_NAME%-%APPVEYOR_REPO_TAG_NAME%.zip build\*
	DIR deploy
)

IF %RUN_TEST%==1 (
	ECHO -- preparing test
	COPY build\* test\
	PUSHD test
	DIR
	ECHO.
	ECHO -- running test
	CALL run_test.bat
	ECHO.
	POPD
) ELSE (
	ECHO -- skipping test
)

IF %KEEPSAMPLE%==1 (
	IF EXIST test\PhnomPenh.osm.pbf (
		ECHO -- [ WARN ] adding sample file to build folder
		COPY test\PhnomPenh.osm.pbf build\PhnomPenh.osm.pbf
	)
) ELSE (
	ECHO -- leaving sample file out of build folder
)

:LEAVE
ECHO -- [ LEAVE ] scripts\build\post.bat
