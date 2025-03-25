robocopy "%1/Detours/src" "%2/src" *.* /S
IF ERRORLEVEL 4 ( EXIT 1 )
robocopy "%1/Detours" "%2" system.mak /S
IF ERRORLEVEL 4 ( EXIT 1 )
git init
git apply "%1/detours_skip_version.patch"