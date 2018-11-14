
@ECHO ON

set DIRENT_REPO_DIR="%BUILD_CACHE_DIR%\dirent"
set GETOPT_REPO_DIR="%BUILD_CACHE_DIR%\getopt"
set CHECK_REPO_DIR="%BUILD_CACHE_DIR%\check"
set CHECK_REPO_BRANCH="windows"
set CHECK_BUILD_DIR="%BUILD_CACHE_DIR%\check\build"
set JSONC_REPO_DIR="%BUILD_CACHE_DIR%\json-c"
set JSONC_REPO_BRANCH="windows"
set JSONC_BUILD_DIR="%BUILD_CACHE_DIR%\json-c\build"
set PCRE_REPO_DIR="%BUILD_CACHE_DIR%\pcre"
set PCRE_REPO_BRANCH="master"
set PCRE_BUILD_DIR="%BUILD_CACHE_DIR%\pcre\build"
set TALLOC_REPO_DIR="%BUILD_CACHE_DIR%\talloc"
set TALLOC_REPO_BRANCH="master"
set TALLOC_BUILD_DIR="%BUILD_CACHE_DIR%\talloc\build"
set YAML_REPO_DIR="%BUILD_CACHE_DIR%\yaml"
set YAML_REPO_BRANCH="master"
set YAML_BUILD_DIR="%BUILD_CACHE_DIR%\yaml\build"
set HANDLEBARS_REPO_DIR="%BUILD_CACHE_DIR%\handlebars"
set HANDLEBARS_REPO_BRANCH="master"
set HANDLEBARS_BUILD_DIR="%BUILD_CACHE_DIR%\handlebars\cmake-build"
set PHP_SDK_DIR="%BUILD_CACHE_DIR%\php-sdk-binary-tools-%PHP_SDK_BINARY_TOOLS_VER%"

if "%1" == "install" (
	if not exist "%BUILD_CACHE_DIR%" (
		mkdir %BUILD_CACHE_DIR%
	)

	REM dirent.h
	if not exist "%DIRENT_REPO_DIR%" (
		git clone https://github.com/jbboehr/dirent.git %DIRENT_REPO_DIR%
	) else (
		cd %DIRENT_REPO_DIR%
		git fetch origin
		git checkout --force origin/master
	)
	echo %DIRENT_REPO_DIR%\include\dirent.h %ARTIFACT_DIR%\include
	copy /Y %DIRENT_REPO_DIR%\include\dirent.h %ARTIFACT_DIR%\include

	REM getopt.h
	if not exist "%GETOPT_REPO_DIR%" (
		git clone https://github.com/jbboehr/Getopt-for-Visual-Studio.git %GETOPT_REPO_DIR%
	) else (
		cd %DIRENT_REPO_DIR%
		git fetch origin
		git checkout --force origin/master
	)
	echo %GETOPT_REPO_DIR%\getopt.h %ARTIFACT_DIR%\include
	copy /Y %GETOPT_REPO_DIR%\getopt.h %ARTIFACT_DIR%\include

	REM check
	if not exist "%CHECK_REPO_DIR%" (
		git clone -b %CHECK_REPO_BRANCH% https://github.com/jbboehr/check.git %CHECK_REPO_DIR%
	) else (
		cd %CHECK_REPO_DIR%
		git fetch origin
		git checkout --force origin/%CHECK_REPO_BRANCH%
	)
	mkdir %CHECK_BUILD_DIR%
	cd %CHECK_BUILD_DIR%
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%ARTIFACT_DIR% ..
	nmake all install

	REM json-c
	if not exist "%JSONC_REPO_DIR%" (
		git clone -b %JSONC_REPO_BRANCH% https://github.com/jbboehr/json-c.git %JSONC_REPO_DIR%
	) else (
		cd %JSONC_REPO_DIR%
		git fetch origin
		git checkout --force origin/%JSONC_REPO_BRANCH%
	)
	mkdir %JSONC_BUILD_DIR%
	cd %JSONC_BUILD_DIR%
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%ARTIFACT_DIR% ..
	nmake all install

	REM pcre
	if not exist "%PCRE_REPO_DIR%" (
		git clone -b %PCRE_REPO_BRANCH% https://github.com/jbboehr/pcre.git %PCRE_REPO_DIR%
	) else (
		cd %PCRE_REPO_DIR%
		git fetch origin
		git checkout --force origin/%PCRE_REPO_BRANCH%
	)
	mkdir %PCRE_BUILD_DIR%
	cd %PCRE_BUILD_DIR%
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%ARTIFACT_DIR% ..
	nmake all install

	REM talloc
	if not exist "%TALLOC_REPO_DIR%" (
		git clone -b %TALLOC_REPO_BRANCH% https://github.com/jbboehr/talloc-win.git %TALLOC_REPO_DIR%
	) else (
		cd %TALLOC_REPO_DIR%
		git fetch origin
		git checkout --force origin/%TALLOC_REPO_BRANCH%
	)
	mkdir %TALLOC_BUILD_DIR%
	cd %TALLOC_BUILD_DIR%
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%ARTIFACT_DIR% ..
	nmake all install

	REM yaml
	if not exist "%YAML_REPO_DIR%" (
		git clone -b %YAML_REPO_BRANCH% https://github.com/yaml/libyaml.git %YAML_REPO_DIR%
	) else (
		cd %YAML_REPO_DIR%
		git fetch origin
		git checkout --force origin/%YAML_REPO_BRANCH%
	)
	mkdir %YAML_BUILD_DIR%
	cd %YAML_BUILD_DIR%
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%ARTIFACT_DIR% ..
	nmake
	copy /Y *.lib %ARTIFACT_DIR%\lib
	copy /Y ..\include\yaml.h %ARTIFACT_DIR%\include

	REM handlebars
	if not exist "%HANDLEBARS_REPO_DIR%" (
		git clone -b %HANDLEBARS_REPO_BRANCH% https://github.com/jbboehr/handlebars.c.git %HANDLEBARS_REPO_DIR%
	) else (
		cd %HANDLEBARS_REPO_DIR%
		git fetch origin
		git checkout --force origin/%HANDLEBARS_REPO_BRANCH%
	)
	mkdir %HANDLEBARS_BUILD_DIR%
	cd %HANDLEBARS_BUILD_DIR%
	cmake -G "NMake Makefiles" -D_CRT_SECURE_NO_WARNINGS=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%ARTIFACT_DIR% -DCMAKE_LIBRARY_PATH=%ARTIFACT_DIR%\lib -DCMAKE_INCLUDE_PATH=%ARTIFACT_DIR%\include ..
	nmake all install

	REM php-sdk
	cd C:\projects
	wget https://github.com/OSTC/php-sdk-binary-tools/archive/%PHP_SDK_BINARY_TOOLS_VER%.zip --no-check-certificate -q -O php-sdk-binary-tools-%PHP_SDK_BINARY_TOOLS_VER%.zip
	7z x -y php-sdk-binary-tools-%PHP_SDK_BINARY_TOOLS_VER%.zip -oC:\projects
	move C:\projects\php-sdk-binary-tools-%PHP_SDK_BINARY_TOOLS_VER% C:\projects\php-sdk

	REM php-src
	cd C:\projects
	wget https://github.com/php/php-src/archive/PHP-%PHP_VER%.zip --no-check-certificate -q -O php-src-%PHP_VER%.zip
	7z x -y php-src-%PHP_VER%.zip -oC:\projects
	move C:\projects\php-src-PHP-%PHP_VER% C:\projects\php-src

	REM php-deps
	cd C:\projects
	wget http://windows.php.net/downloads/php-sdk/deps-%PHP_DEPS_VER%-%VC_VER%-%PLATFORM%.7z -q
	7z x -y deps-%PHP_DEPS_VER%-%VC_VER%-%PLATFORM%.7z -oC:\projects\php-src
)

if "%1" == "build_script" (
	cd C:\projects\php-src
	C:\projects\php-sdk\bin\phpsdk_setvars.bat
	mkdir C:\projects\php-src\ext\handlebars
	xcopy %APPVEYOR_BUILD_FOLDER% C:\projects\php-src\ext\handlebars /s /e /y /q
	buildconf.bat
	configure.bat --disable-all --enable-cli --enable-cgi --enable-zts --enable-json --enable-handlebars=shared --with-libhandlebars=%ARTIFACT_DIR% --with-prefix=%ARTIFACT_DIR%\bin --with-php-build=deps
	nmake
	nmake install
	cd %ARTIFACT_DIR%\bin
	echo [PHP] > php.ini
	echo extension_dir = "ext" >> php.ini
	echo extension=php_handlebars.dll >> php.ini
)

cd %APPVEYOR_BUILD_FOLDER%
