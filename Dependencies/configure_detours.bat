robocopy "%1/src" "%2/src" *.* /S
IF ERRORLEVEL 4 ( EXIT 1 )
robocopy "%1" "%2" system.mak /S
IF ERRORLEVEL 4 ( EXIT 1 )
git init
git apply %3/Patches/detours_skip_version.patch