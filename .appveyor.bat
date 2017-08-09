
if not exist "%BUILD_CACHE_DIR%" (
	mkdir %BUILD_CACHE_DIR%
)

if "%1" == "install" (
	REM dirent.h
	set DIRENT_REPO_DIR="%BUILD_CACHE_DIR%\dirent"
	if not exist "%DIRENT_REPO_DIR%" (
		git clone https://github.com/jbboehr/dirent.git %DIRENT_REPO_DIR%
	) else (
		cd %DIRENT_REPO_DIR%
		git fetch origin
		git checkout --force origin/master
	)
	copy /Y %DIRENT_REPO_DIR%\include\dirent.h %ARTIFACT_DIR%\include

	REM getopt.h
	set GETOPT_REPO_DIR="%BUILD_CACHE_DIR%\getopt"
	if not exist "%GETOPT_REPO_DIR%" (
		git clone https://github.com/jbboehr/Getopt-for-Visual-Studio.git %GETOPT_REPO_DIR%
	) else (
		cd %DIRENT_REPO_DIR%
		git fetch origin
		git checkout --force origin/master
	)
	copy /Y %GETOPT_REPO_DIR%\getopt.h %ARTIFACT_DIR%\include
)

