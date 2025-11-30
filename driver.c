#include <windows.h>
#include <KnownFolders.h>
#include <Shlobj.h>
#include <stdio.h>


char cbuff[1024];
WCHAR wbuff[1024];

void test1() {
  const char *keys[] = {"appData", "localAppData", "EMROOT"};
  for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
    const char *key = keys[i];
    int gev = GetEnvironmentVariableA(key, cbuff, sizeof(cbuff));
    if (gev != 0) {
      printf("%s: %s\n", key, cbuff);
    }
  }
}

void test2() {
  const WCHAR *keys[] = {L"appData", L"localAppData", L"EMROOT"};
  for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
    int gev = GetEnvironmentVariableW(keys[i], wbuff, sizeof(wbuff));
    if (gev != 0) {
      wprintf(L"wide %s: %s\n", keys[i], wbuff);
    }
  }
}

void test3() {
  LPWCH envStrings = GetEnvironmentStringsW();
  if (envStrings == NULL) {
    wprintf(L"GetEnvironmentStringsW failed.\n");
    return;
  }

  LPWCH current = envStrings;

  while (*current) {
    if (wcslen(current) < 50) {
      wprintf(L"%s\n", current);
    }
    current += wcslen(current) + 1;
  }

  if (!FreeEnvironmentStringsW(envStrings)) {
    wprintf(L"FreeEnvironmentStringsW failed.\n");
  }
}

void test4() {
  HRESULT res;
  PWSTR path = 0;
  res = SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, 0, &path);
  if (res == S_OK) {
    wprintf(L"SHGetKnownFolderPath(RoamingAppData): %s\n", path);
  }
  res = SHGetKnownFolderPath(&FOLDERID_LocalAppData, 0, 0, &path);
  if (res == S_OK) {
    wprintf(L"SHGetKnownFolderPath(LocalAppData): %s\n", path);
  }
  res = SHGetKnownFolderPath(&FOLDERID_PublicDocuments, 0, 0, &path);
  if (res == S_OK) {
    wprintf(L"SHGetKnownFolderPath(PublicDocuments): %s\n", path);
  }
}

void test5() {
  {
    if (ExpandEnvironmentStringsA("%USERPROFILE%\\AppData\\Roaming", cbuff, sizeof(cbuff)) == 0) {
      printf("Failed to expand %%APPDATA%%. Error: %lu\n", GetLastError());
      return;
    }
    printf("ExpandEnvironmentStringsA AppDataRoaming: %s\n", cbuff);
  }
  {
    if (ExpandEnvironmentStringsW(L"%USERPROFILE%\\AppData\\Roaming", wbuff, sizeof(wbuff)) == 0) {
      printf("Failed to expand %%APPDATA%%. Error: %lu\n", GetLastError());
      return;
    }
    wprintf(L"ExpandEnvironmentStringsW AppDataRoaming: %s\n", wbuff);
  }
}

void test6() {
  HRESULT res;
  res = SHGetFolderPathA(0, CSIDL_APPDATA, 0, 0, cbuff);
  if (res == S_OK) {
    printf("SHGetFolderPathA(RoamingAppData): %s\n", cbuff);
  }
  res = SHGetFolderPathA(0, CSIDL_DESKTOP, 0, 0, cbuff);
  if (res == S_OK) {
    printf("SHGetFolderPathA(Desktop): %s\n", cbuff);
  }
  res = SHGetFolderPathW(0, CSIDL_APPDATA, 0, 0, wbuff);
  if (res == S_OK) {
    wprintf(L"SHGetFolderPathW(RoamingAppData): %s\n", wbuff);
  }
  res = SHGetFolderPathW(0, CSIDL_DESKTOP, 0, 0, wbuff);
  if (res == S_OK) {
    wprintf(L"SHGetFolderPathW(Desktop): %s\n", wbuff);
  }
}

int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
}