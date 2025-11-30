cl /nologo /c gev.c /I Detours\include 
link /nologo /DLL /DEF:gev.def /OUT:gev_shared.dll gev.obj Detours\lib.X64\detours.lib shell32.lib
cl /nologo driver.c shell32.lib /Fe:driver.exe
.\Detours\bin.X64\setdll.exe -d:gev_shared.dll driver.exe