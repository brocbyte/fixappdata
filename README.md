## Description

This is a command line utility for Windows that allows to somewhat change the APPDATA/LOCALAPPDATA folder of a target application.

Usually %APPDATA% is pointing to C:/Users/username/AppData/Roaming
and %LOCALAPPDATA% is pointing to C:/Users/username/AppData/Local

This utility allows you to redirect these.

## Build

Open a Developer Command Prompt for VS. To target x64, choose the "X64 Native Tools Command Prompt for VS".

Run `git clone --recurse-submodules https://github.com/brocbyte/fixappdata.git && cd fixappdata`

To build the utility itself and a sample target 'driver': `nmake`

## Usage

`patch.bat` is used to set the appdata-related folders for the specific application. It can be called like that:

`patch.bat "C:\\newappdata" "C:\\newlocalappdata" "C:\\target_app.exe"`

This command will patch the `target_app.exe` application (and save the old version as `~target_app.exe`).
The `gev_shared.dll` library will be copied into the folder containing target application.

## Details

It works by partially overriding the behaviour of the Win32 API functions related to environment variables / folders.

The hard part (dll function hooking) is done by absolutely amazing Detours library from Microsoft (https://github.com/microsoft/Detours).

There are several calls that are usually used for retrieving APPDATA/LOCALAPPDATA:

- `GetEnvironmentStringsW`
- `GetEnvironmentVariableW`/`GetEnvironmentVariableA`
- `ExpandEnvironmentStringsW`/`ExpandEnvironmentStringsA`
- `SHGetKnownFolderPath`
- `SHGetFolderPathW`/`SHGetFolderPathA`
- ... and multiple other quirks, cause WinAPI is _vintage_

This application overrides ^^ them with its own implementation, which handles APPDATA-related stuff specifically while passing all irrelevant calls into the original functions.
