#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "test.h"

int main(int argc, char *argv[]) {
    Array a = NewArray((const void **)argv);

    char *str = a.Join(&a, " ");
    InitCLP(str);
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