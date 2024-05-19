#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

typedef void (*ModuleFunction)();

typedef struct {
    void *handle;
    char *name;
} LoadedModule;

#define MAX_MODULES 10

LoadedModule loadedModules[MAX_MODULES];
int numLoadedModules = 0;

void loadModule(const char *moduleName);
void unloadModule(const char *moduleName);
void callFunction(const char *moduleName, const char *functionName);

int main() {
    char command[100];
    char moduleName[100];
    char functionName[100];

    while (1) {
        printf(">> ");
        scanf("%s", command);

        if (strcmp(command, "load_module") == 0) {
            scanf("%s", moduleName);
            loadModule(moduleName);
        } else if (strcmp(command, "unload_module") == 0) {
            scanf("%s", moduleName);
            unloadModule(moduleName);
        } else if (strcmp(command, "call_function") == 0) {
            scanf("%s %s", moduleName, functionName);
            callFunction(moduleName, functionName);
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Unknown command\n");
        }
    }

    return 0;
}

void loadModule(const char *moduleName) {
    if (numLoadedModules >= MAX_MODULES) {
        printf("Cannot load module. Maximum number of modules reached.\n");
        return;
    }

    for (int i = 0; i < numLoadedModules; i++) {
        if (strcmp(loadedModules[i].name, moduleName) == 0) {
            printf("Module '%s' is already loaded.\n", moduleName);
            return;
        }
    }

    void *handle = dlopen(moduleName, RTLD_NOW);
    if (handle == NULL) {
        printf("Failed to load module '%s': %s\n", moduleName, dlerror());
        return;
    }

    loadedModules[numLoadedModules].handle = handle;
    loadedModules[numLoadedModules].name = strdup(moduleName);
    numLoadedModules++;

    printf("Module '%s' loaded successfully.\n", moduleName);
}

void unloadModule(const char *moduleName) {
    for (int i = 0; i < numLoadedModules; i++) {
        if (strcmp(loadedModules[i].name, moduleName) == 0) {
            dlclose(loadedModules[i].handle);
            printf("Module '%s' unloaded successfully.\n", moduleName);

            for (int j = i; j < numLoadedModules - 1; j++) {
                loadedModules[j] = loadedModules[j + 1];
            }
            numLoadedModules--;
            return;
        }
    }

    printf("Module '%s' is not loaded.\n", moduleName);
}

void callFunction(const char *moduleName, const char *functionName) {
    for (int i = 0; i < numLoadedModules; i++) {
        if (strcmp(loadedModules[i].name, moduleName) == 0) {
            ModuleFunction func = dlsym(loadedModules[i].handle, functionName);
            if (func == NULL) {
                printf("Function '%s' not found in module '%s'\n", functionName, moduleName);
                return;
            }

            func();
            return;
        }
    }

    printf("Module '%s' is not loaded.\n", moduleName);
}
