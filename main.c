#include <stdio.h>
#include <Windows.h>

// Функція для отримання значення з реєстру
char* GetRegistryValue(HKEY hKey, const char* subKey, const char* valueName) {
    HKEY hOpenKey;
    if (RegOpenKeyExA(hKey, subKey, 0, KEY_READ, &hOpenKey) == ERROR_SUCCESS) {
        DWORD dwType;
        DWORD dwSize = 0;
        if (RegQueryValueExA(hOpenKey, valueName, NULL, &dwType, NULL, &dwSize) == ERROR_SUCCESS) {
            if (dwType == REG_SZ || dwType == REG_EXPAND_SZ) {
                char* value = (char*)malloc(dwSize);
                if (RegQueryValueExA(hOpenKey, valueName, NULL, NULL, (LPBYTE)value, &dwSize) == ERROR_SUCCESS) {
                    RegCloseKey(hOpenKey);
                    return value;
                }
                free(value);
            }
        }
        RegCloseKey(hOpenKey);
    }
    return NULL;
}

int main() {
    FILE* outFile = fopen("system_info.txt", "w");
    if (outFile == NULL) {
        fprintf(stderr, "Unable to open file.\n");
        return 1;
    }

    // Отримуємо інформацію про комп'ютер
    fprintf(outFile, "Computer Name: %s\n", GetRegistryValue(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName", "ComputerName"));
    fprintf(outFile, "Windows Version: %s %s\n", GetRegistryValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName"), GetRegistryValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CurrentVersion"));
    fprintf(outFile, "Processor: %s\n", GetRegistryValue(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "ProcessorNameString"));

    fclose(outFile);
    return 0;
}
