#include <stdio.h>
#include <windows.h>
#include <KnownFolders.h>
#include <Shlobj_core.h>
#include <detours.h>


#include <wchar.h>
#include <wctype.h>

static LPWCH(WINAPI *TrueGetEnvironmentStringsW)(void) = GetEnvironmentStringsW;
static DWORD(WINAPI *TrueGetEnvironmentVariableW)(
    LPCWSTR lpName, LPWSTR lpBuffer, DWORD nSize) = GetEnvironmentVariableW;
static DWORD(WINAPI *TrueGetEnvironmentVariableA)(
    LPCSTR lpName, LPSTR lpBuffer, DWORD nSize) = GetEnvironmentVariableA;

static DWORD(WINAPI *TrueExpandEnvironmentStringsA)(
    LPCSTR lpSrc, LPSTR lpDst, DWORD nSize) = ExpandEnvironmentStringsA;
static DWORD(WINAPI *TrueExpandEnvironmentStringsW)(
    LPCWSTR lpSrc, LPWSTR lpDst, DWORD nSize) = ExpandEnvironmentStringsW;

static HRESULT(WINAPI *TrueSHGetKnownFolderPath)(
    REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken,
    PWSTR *ppszPath) = SHGetKnownFolderPath;

static HRESULT(STDAPICALLTYPE *TrueSHGetFolderPathA)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath) = SHGetFolderPathA;
static HRESULT(STDAPICALLTYPE *TrueSHGetFolderPathW)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath) = SHGetFolderPathW;

const char *aGetEnvKeys[] = {"appdata", "localappdata"};
const char *aGetEnvValues[] = {"C:\\mine\\appdota\\Roaming",
                               "C:\\mine\\appdota\\Local"};

const WCHAR *wGetEnvKeys[] = {L"appdata", L"localappdata"};
const WCHAR *wGetEnvValues[] = {L"C:\\mine\\appdota\\Roaming",
                                L"C:\\mine\\appdota\\Local"};

const WCHAR *gesKeys[] = {L"APPDATA=", L"LOCALAPPDATA"};
const WCHAR *gesValues[] = {L"APPDATA=C:\\mine\\appdota\\Roaming",
                            L"LOCALAPPDATA=C:\\mine\\appdota\\Local"};

LPWCH WINAPI MineGetEnvironmentStringsW(void) {
  LPWCH env = TrueGetEnvironmentStringsW();
  if (!env)
    return NULL;

  LPWCH r = env;
  size_t total = 0;
  while (*r) {
    size_t len = wcslen(r) + 1;
    total += len;
    r += len;
  }
  total++;

  LPWCH copy = (LPWCH)HeapAlloc(GetProcessHeap(), 0, total * sizeof(WCHAR));
  if (!copy) {
    FreeEnvironmentStringsW(env);
    return NULL;
  }

  r = env;
  LPWCH w = copy;

  while (*r) {
    size_t len = wcslen(r);

    int found = 0;
    for (int i = 0; i < sizeof(gesKeys) / sizeof(gesKeys[0]); ++i) {
      const WCHAR *key = gesKeys[i];
      size_t keyLen = wcslen(key);
      if (_wcsnicmp(r, key, keyLen) == 0) {
        const WCHAR *newVal = gesValues[i];
        size_t newLen = wcslen(newVal);

        if (newLen <= len) {
          memcpy(w, newVal, (newLen + 1) * sizeof(WCHAR));
          w += newLen + 1;
          found = 1;
          break;
        }
      }
    }
    if (!found) {
      memcpy(w, r, (len + 1) * sizeof(WCHAR));
      w += len + 1;
    }

    r += len + 1;
  }

  FreeEnvironmentStringsW(env);
  return copy;
}

DWORD WINAPI MineGetEnvironmentVariableW(LPCWSTR lpName, LPWSTR lpBuffer,
                                         DWORD nSize) {
  for (int ti = 0; ti < sizeof(wGetEnvKeys) / sizeof(wGetEnvKeys[0]); ++ti) {
    const WCHAR *target = wGetEnvKeys[ti];
    if (_wcsicmp(lpName, target) == 0) {
      return (DWORD)swprintf(lpBuffer, nSize, wGetEnvValues[ti]);
    }
  }
  return TrueGetEnvironmentVariableW(lpName, lpBuffer, nSize);
}

DWORD WINAPI MineGetEnvironmentVariableA(char *lpName, char *lpBuffer,
                                         DWORD nSize) {
  for (int ti = 0; ti < sizeof(aGetEnvKeys) / sizeof(aGetEnvKeys[0]); ++ti) {
    const char *target = aGetEnvKeys[ti];
    const char *p1 = lpName;
    const char *p2 = target;
    int eq = 1;
    if (_stricmp(lpName, target) == 0) {
      return (DWORD)snprintf(lpBuffer, nSize, aGetEnvValues[ti]);
    }
  }
  return TrueGetEnvironmentVariableA(lpName, lpBuffer, nSize);
}

HRESULT WINAPI MineSHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags,
                                        HANDLE hToken, PWSTR *ppszPath) {
  if (IsEqualGUID(rfid, &FOLDERID_RoamingAppData) == TRUE) {
    size_t len = wcslen(wGetEnvValues[0]) + 1;
    LPWCH storage = (LPWCH)HeapAlloc(GetProcessHeap(), 0, len * sizeof(WCHAR));
    swprintf(storage, len, wGetEnvValues[0]);
    *ppszPath = storage;
    return S_OK;
  }
  if (IsEqualGUID(rfid, &FOLDERID_LocalAppData) == TRUE) {
    size_t len = wcslen(wGetEnvValues[1]) + 1;
    LPWCH storage = (LPWCH)HeapAlloc(GetProcessHeap(), 0, len * sizeof(WCHAR));
    swprintf(storage, len, wGetEnvValues[1]);
    *ppszPath = storage;
    return S_OK;
  }
  return TrueSHGetKnownFolderPath(rfid, dwFlags, hToken, ppszPath);
}

DWORD WINAPI MineExpandEnvironmentStringsA(LPCSTR lpSrc, LPSTR lpDst,
                                           DWORD nSize) {
    if (_stricmp(lpSrc, "%USERPROFILE%\\AppData\\Roaming") == 0) {
      return snprintf(lpDst, nSize, aGetEnvValues[0]);
    }
    return TrueExpandEnvironmentStringsA(lpSrc, lpDst, nSize);
}

DWORD WINAPI MineExpandEnvironmentStringsW(LPCWSTR lpSrc, LPWSTR lpDst, DWORD nSize) {
    if (_wcsicmp(lpSrc, L"%USERPROFILE%\\AppData\\Roaming") == 0) {
      return swprintf(lpDst, nSize, wGetEnvValues[0]);
    }
    return TrueExpandEnvironmentStringsW(lpSrc, lpDst, nSize);
}

HRESULT STDAPICALLTYPE MineSHGetFolderPathA(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath) {
  if (csidl == CSIDL_APPDATA) {
    sprintf(pszPath, aGetEnvValues[0]);
    return S_OK;
  }
  return TrueSHGetFolderPathA(hwnd, csidl, hToken, dwFlags, pszPath);
}

HRESULT STDAPICALLTYPE MineSHGetFolderPathW(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath) {
  if (csidl == CSIDL_APPDATA) {
    swprintf(pszPath, MAX_PATH, wGetEnvValues[0]);
    return S_OK;
  }
  return TrueSHGetFolderPathW(hwnd, csidl, hToken, dwFlags, pszPath);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  if (DetourIsHelperProcess()) {
    return TRUE;
  }

  if (dwReason == DLL_PROCESS_ATTACH) {
    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID *)&TrueGetEnvironmentVariableA,
                 MineGetEnvironmentVariableA);
    DetourAttach((PVOID *)&TrueGetEnvironmentVariableW,
                 MineGetEnvironmentVariableW);
    DetourAttach((PVOID *)&TrueGetEnvironmentStringsW,
                 MineGetEnvironmentStringsW);
    DetourAttach((PVOID *)&TrueSHGetKnownFolderPath, MineSHGetKnownFolderPath);
    DetourAttach((PVOID *)&TrueExpandEnvironmentStringsA, MineExpandEnvironmentStringsA);
    DetourAttach((PVOID *)&TrueExpandEnvironmentStringsW, MineExpandEnvironmentStringsW);
    DetourAttach((PVOID *)&TrueSHGetFolderPathA, MineSHGetFolderPathA);
    DetourAttach((PVOID *)&TrueSHGetFolderPathW, MineSHGetFolderPathW);
    DetourTransactionCommit();
  } else if (dwReason == DLL_PROCESS_DETACH) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach((PVOID *)&TrueGetEnvironmentVariableA,
                 MineGetEnvironmentVariableA);
    DetourDetach((PVOID *)&TrueGetEnvironmentVariableW,
                 MineGetEnvironmentVariableW);
    DetourDetach((PVOID *)&TrueGetEnvironmentStringsW,
                 MineGetEnvironmentStringsW);
    DetourDetach((PVOID *)&TrueSHGetKnownFolderPath, MineSHGetKnownFolderPath);
    DetourDetach((PVOID *)&TrueExpandEnvironmentStringsA, MineExpandEnvironmentStringsA);
    DetourDetach((PVOID *)&TrueExpandEnvironmentStringsW, MineExpandEnvironmentStringsW);
    DetourDetach((PVOID *)&TrueSHGetFolderPathA, MineSHGetFolderPathA);
    DetourDetach((PVOID *)&TrueSHGetFolderPathW, MineSHGetFolderPathW);
    DetourTransactionCommit();
  }
  return TRUE;
}