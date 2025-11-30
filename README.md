## Description

This is a console application that allows you to change the APPDATA/LOCALAPPDATA folder of another application (usually C:/Users/username/AppData/Roaming and C:/Users/username/AppData/Local).
It works by partially overriding the behaviour of the Win32 API functions related to environment variables / folders.
The hard part (dll function hooking) is done by absolutely amazing Detours library from Microsoft (https://github.com/microsoft/Detours).

There are several calls that are usually used for retrieving APPDATA/LOCALAPPDATA:

- GetEnvironmentStringsW
- GetEnvironmentVariableW/GetEnvironmentVariableA
- ExpandEnvironmentStringsW/ExpandEnvironmentStringsA
- SHGetKnownFolderPath
- SHGetFolderPathW/SHGetFolderPathA
- ... and multiple other quirks, cause WinAPI is _vintage_

This application overrides ^^ them with its own implementation, which handles APPDATA-related stuff specifically while passing all irrelevant calls into the original functions.
