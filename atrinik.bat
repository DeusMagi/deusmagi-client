@echo off

echo Updating Deus Magi installation, please wait...

rem Make sure we are running in the directory the Batch file is in.
cd "%0\.."
cd /d "%0\.."

rem Wait a few seconds to make sure the updater has finished.
timeout /NOBREAK 2

rem Make sure no clients are running.
taskkill /f /t /im deusmagi.exe >nul 2>&1

rem Store the current working directory.
set old_dir=%CD%
rem Go to the patches directory.
cd "%AppData%\.deusmagi\temp"

rem Extract all patches.
for %%f in (*.tar.gz) do (
	echo Extracting %%f
	"%old_dir%\gunzip.exe" -c %%f > %%~nf
	"%old_dir%\tar.exe" xvf %%~nf
	del /q %%f
	del /q %%~nf
)

rem Go back to the old directory.
cd "%old_dir%"
rem Copy over the extracted files.
xcopy /s/e/y "%AppData%"\.deusmagi\temp\*.* .\
rem Remove the temporary directory.
rmdir /s/q "%AppData%"\.deusmagi\temp

rem Start up the client.
start deusmagi.exe %*
exit
