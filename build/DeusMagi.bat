@ECHO OFF

START "Deus Magi" wsl ./DeusMagi.AppImage &

IF %ERRORLEVEL% NEQ 0 (
    ECHO:
    ECHO You must install WSL v2 and all dependencies before running Deus Magi ...
    ECHO:
    ECHO wsl --install
    ECHO:
    ECHO Please see the Client Requirements in the README for a list of the dependencies.
)