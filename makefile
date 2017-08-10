
# FIXME: All this is rather hacky . I will need to move
#	most of the logic inside the appveyor.bat . Also make
#	use of templates .

SHELL := cmd.exe
TEST_PBF_URL := http://download.bbbike.org/osm/bbbike/PhnomPenh/PhnomPenh.osm.pbf

.PHONY: build
build:
	make -C src
	IF NOT EXIST build MKDIR build
	COPY src\protocasm.exe build\protocasm.exe
	python src\patch.py src\osm.protocasm -f -o build\osm.protocasm -b Ii4uXGRhdGFcRGFybXN0YWR0Lm9zbS5wYmYi IlBobm9tUGVuaC5vc20ucGJmIg==
	ECHO IF NOT EXIST ^"%%~dp0$(notdir $(TEST_PBF_URL))^" wget -P ^"%%~dp0.^" --no-check-certificate $(TEST_PBF_URL) > build\fetch_sample.bat
	ECHO CALL ^"%%~dp0fetch_sample.bat^" > build\run_test.bat
	ECHO ^"%%~dp0protocasm.exe^" ^"%%~dp0osm.protocasm^" >> build\run_test.bat
	ECHO [InternetShortcut] > build\github.url
	ECHO URL=https://github.com/dejbug/protocasm >> build\github.url

.PHONY: run
run: build
	IF NOT EXIST test MKDIR test
	COPY build\protocasm.exe test\protocasm.exe
	python src\patch.py src\osm.protocasm -f -o test\osm.protocasm -b Ii4uXGRhdGFcRGFybXN0YWR0Lm9zbS5wYmYi InRlc3RcUGhub21QZW5oLm9zbS5wYmYi
	ECHO test\protocasm.exe test\osm.protocasm > test\run_test.bat
	IF NOT EXIST test\$(notdir $(TEST_PBF_URL)) wget -P test --no-check-certificate $(TEST_PBF_URL)
	test\protocasm.exe test\osm.protocasm

.PHONY: clean
clean:
	make -C src clean

.PHONY: reallyclean
reallyclean:
	make -C src clean
	IF EXIST build RMDIR /S /Q build
	IF EXIST test RMDIR /S /Q test

