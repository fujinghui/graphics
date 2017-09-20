#include <windows.h>
void __declspec(dllexport) HelloWorld();
int __declspec(dllexport) SetKeyHook(int bInstall, DWORD dwThreadId, HWND hWndCalller);