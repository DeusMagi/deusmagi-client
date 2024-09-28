/**
 * @file
 * Handles the Windows client wrapper. Basically this updater is executed
 * by shortcuts instead of deusmagi.exe in order to handle extracting
 * update patches.
 *
 * @author Alex Tokar
 */

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <direct.h>
#include <shellapi.h>

#define HUGE_BUF 4096 * 12

int main(int argc, char *argv[])
{
    char params[HUGE_BUF], path[HUGE_BUF], wdir[HUGE_BUF];
    int i;

    params[0] = '\0';

    /* Construct the command parameters from arguments. */
    for (i = 1; i < argc; i++) {
        strncat(params, argv[i], sizeof(params) - strlen(params) - 1);
    }

    /* Check if we have upgrades to apply. */
    snprintf(path, sizeof(path), "%s/.deusmagi/temp", getenv("APPDATA"));

    if (access(path, R_OK) == 0) {
        SHELLEXECUTEINFO shExecInfo;

        shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        shExecInfo.fMask = 0;
        shExecInfo.hwnd = NULL;
        shExecInfo.lpVerb = "runas";
        shExecInfo.lpFile = "deusmagi.bat";
        shExecInfo.lpParameters = params;
        shExecInfo.lpDirectory = NULL;
        shExecInfo.nShow = SW_SHOW;
        shExecInfo.hInstApp = NULL;

        ShellExecuteEx(&shExecInfo);
    } else {
        /* No updates, execute the client. */
        snprintf(path, sizeof(path), "%s\\deusmagi.exe", getcwd(wdir, sizeof(wdir) - 1));
        ShellExecute(NULL, "open", path, params, NULL, SW_SHOWNORMAL);
    }

    return 0;
}
