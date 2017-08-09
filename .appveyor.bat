
@ECHO ON

set DIRENT_REPO_DIR="%BUILD_CACHE_DIR%\dirent"
set GETOPT_REPO_DIR="%BUILD_CACHE_DIR%\getopt"
set CHECK_REPO_DIR="%BUILD_CACHE_DIR%\check"
set CHECK_REPO_BRANCH="windows"
set CHECK_BUILD_DIR="%CHECK_SRC_DIR%\build"

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
	
)

