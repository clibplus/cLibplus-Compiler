#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "clibp.h"

int arr_contains(char **argv, char *q) {
    if(!argv || !q)
        return 0;

    for(int i = 0; argv[i] != NULL; i++)
        if(!strcmp(argv[i], q))
            return 1;

    return 0;
}

int main(int argc, char *argv[]) {
    Array a = NewArray((const void **)argv);

    char *str = a.Join(&a, " ");
    InitCLP(str, arr_contains(argv, "--debug"));
    // void *handle = dlopen("./libshared.so", RTLD_NOW);
    // if (!handle) {
    //     fprintf(stderr, "dlopen failed: %s\n", dlerror());
    //     return 1;
    // }

    // int *shared_value = (int *)dlsym(handle, "shared_value");
    // if (!shared_value) {
    //     fprintf(stderr, "dlsym failed: %s\n", dlerror());
    //     return 1;
    // }

    // printf("Original value: %d\n", *shared_value);

    // *shared_value = 123; // ✅ Set the variable
    // printf("New value: %d\n", *shared_value);

    // dlclose(handle);
    return 0;
}