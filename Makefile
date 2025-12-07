CC = cl
LD = link
SETDLL = Detours\bin.X64\setdll.exe

LDFLAGS_DLL = /nologo /DLL /DEF:gev.def
LDFLAGS_EXE = /nologo

OBJS = gev.obj
DLL_NAME = gev_shared.dll
EXE_NAME = driver.exe

DLL_LIBS = Detours\lib.X64\detours.lib shell32.lib
EXE_LIBS = shell32.lib

all: $(SETDLL) $(DLL_NAME) $(EXE_NAME) patched_driver

$(SETDLL): Detours\samples\Makefile
	pushd Detours && nmake && pushd samples && nmake && popd && popd

gev.obj: gev.c
	$(CC) /nologo /c gev.c /I Detours\include

$(DLL_NAME): $(OBJS) gev.def
	$(LD) $(LDFLAGS_DLL) /OUT:$(DLL_NAME) $(OBJS) $(DLL_LIBS)

$(EXE_NAME): driver.c
	$(CC) /nologo driver.c $(EXE_LIBS) /Fe:$(EXE_NAME)

patched_driver: $(EXE_NAME) $(DLL_NAME) $(SETDLL)
	$(SETDLL) -d:$(DLL_NAME) $(EXE_NAME)

clean:
	del /Q *.obj *.dll *.exe *.exp *.lib *.pdb 2>NUL
