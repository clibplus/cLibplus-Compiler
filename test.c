#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "test.h"

cLibp *__Main__;

// void ___type___(void *x) {
//     for(int i = 0; i < __Main__->SourceFiles.idx; i++) {
//         for(int c = 0; c < ((CodeFile *)__Main__->SourceFiles.arr[i])->Vars.idx; c++) {
//             if(((___Variable___ *)((CodeFile *)__Main__->SourceFiles.arr[i])->Vars.arr[i])->Pointer == x)
//                 return ((___Variable___ *)((CodeFile *)__Main__->SourceFiles.arr[i])->Vars.arr[i])->Name;
//         }
//     }
// }

/*
    FUTURE IMPLEMENTATION:
*/
cLibp *InitCLP(const char *cmd) {
    cLibp *p = (cLibp *)malloc(sizeof(cLibp));
    if(!p) {
        printf("[ x ] Error, Unable to allocate memory for cLib+ .....!\n");
        exit(0);
    }

    *p = (cLibp){
        .Files          = NewArray(NULL),
        .SourceFiles    = NewArray(NULL),
        .CompileCmd     = NewString(strdup("gcc ")),
        .AutoFree       = 0,
        .CAutoFree      = 0
    };
    __Main__ = p;

    if(CheckCmd(p, cmd) < 0) {
        printf("[ - ] Error, Invalid arguments supplied....!\n");
        return NULL;
    }

    Parse_cLibp(p);

    return p;
}

int CheckCmd(cLibp *c, const char *cmd) {
    String r = NewString(strdup(cmd));
    Array g = NewArray((const void **)r.Split(&r, " "));
    c->CmdArgs = NewArray((const void **)r.Split(&r, " "));
    
    int found = 0;
    for(int i = 0; i < g.idx; i++) {
        String n = NewString(g.arr[i]);
        if(n.Is(&n, "./clp") || n.Is(&n, "/root/clp")) {
            c->CmdArgs.Remove(&c->CmdArgs, i);
            found = 1;
        }

        if(n.Is(&n, "--autofree")) {
            c->AutoFree = 1;
            c->CmdArgs.Remove(&c->CmdArgs, i);
        }

        if(n.EndsWith(&n, ".clp"))
            c->Files.Append(&c->Files, strdup(n.data));
        else
            printf("Fail %s\n", n.data);

        // if(n.Is(&n, "--c-autofree")) {}
        c->CompileCmd.AppendArray(&c->CompileCmd, (const char *[]){strdup(n.data), " ", NULL});
        n.Destruct(&n);
    }

    if(!found) {
        printf("[ - ] CLP command not used!\n");
        exit(0);
    }

    r.Destruct(&r);
    return c->CmdArgs.idx;
}

void Parse_cLibp(cLibp *c) {
    for(int i = 0; i < c->Files.idx; i++) {
        if(strstr(c->Files.arr[i], ".clp"))
            __Parse_File(c, c->Files.arr[i], (i == 0 ? 1 : 0));
    }
}

int __Parse_File(cLibp *c, const char *file, int main) {
    CodeFile *codefile = (CodeFile *)malloc(sizeof(CodeFile));
    *codefile = (CodeFile){
        .Filepath = strdup(file),
        .Vars = NewArray(NULL),
        .NewContent = Openfile(CreateString((char *[]){".", (char *)file, ".c", NULL}), FILE_WRITE_READ)
    };

    if(!c || !file)
        return -1;

    printf("%s\n", file);
    File Content = Openfile(file, FILE_READ);
    if(!Content.fd) {
        printf("[ - ] Error, Unable to open or read file: %s....!\n", file);
        Content.Destruct(&Content);
        return -1;
    }

    String code = NewString(Content.Read(&Content));
    if(code.idx < 0) {
        printf("[ - ] Error, No code provided in file....!\n");
        Content.Destruct(&Content);
        code.Destruct(&code);
        return -1;
    }

    Array lines = NewArray((const void **)code.Split(&code, "\n"));
    int main_found = 0;
    for(char i = 0, *token = lines.arr[i]; i < lines.idx; i++, token = lines.arr[i]) {
        String line = NewString(token);
        if(line.StartsWith(&line, "int main")) {
            main_found = 1;
        }

        if(line.Contains(&line, "___Any___")) {
            printf("___Any___ Found On %d\n", i);
            Array new_line = __Parse_Any(c, codefile, line);
            char *g = new_line.Join(&new_line, "\n");
            codefile->NewContent.Write(&codefile->NewContent, g);
            codefile->NewContent.Write(&codefile->NewContent, "\n");

            if(new_line.idx > 0) {
                new_line.Destruct(&new_line, 1, 1);
                free(g);
            }
            continue;
        }

        codefile->NewContent.Write(&codefile->NewContent, line.data);
        codefile->NewContent.Write(&codefile->NewContent, "\n");
    }

    char *test = codefile->NewContent.Read(&codefile->NewContent);
    printf("%s\n", test);
    codefile->NewContent.Destruct(&codefile->NewContent);
    Content.Destruct(&Content);
    c->SourceFiles.Append(&c->SourceFiles, codefile);
    printf("%ld\n", c->SourceFiles.idx);
    return 1;
}

Array __Parse_Any(cLibp *c, CodeFile *codefile, String line) {
    ___Variable___ *v = (___Variable___ *)malloc(sizeof(___Variable___));
    *v = (___Variable___){};
    Array new_code = NewArray(NULL);

    int b;
    if((b = line.CountChar(&line, '\t')) > 0)
        for(int n = 0; n < b; n++)
            line.Trim(&line, '\t');

    Array LineArgs = NewArray((const void **)line.Split(&line, " "));
    if(LineArgs.idx < 4) {
        printf("[ - ] Error, Invalid syntax for ___Any___....!\n");
        exit(0);
    }

    char *var_name = LineArgs.arr[1];
    String possible1 = NewString(strdup(var_name));
    possible1.AppendString(&possible1, " = 0;");

    String possible2 = NewString(strdup(var_name));
    possible2.AppendString(&possible2, " = NULL;");

    if(line.EndsWith(&line, possible1.data) || line.EndsWith(&line, possible2.data)) {
        printf("[ - ] Error, Invalid Ending Syntax....!\n");
        exit(0);
    }
    possible1.Destruct(&possible1);
    possible2.Destruct(&possible2);
    v->Name = strdup(var_name);

    /* Create a pointer as stack usage, create variable and set info, add to source code info */
    if(line.Contains(&line, "___Any___")) { // Stack
        v->Pointer = (void *)malloc(sizeof(void *));
        memset(v->Pointer, '\0', sizeof(void *));
        codefile->Vars.Append(&codefile->Vars, v);
        char *num = iString(__Main__->SourceFiles.idx);
        char *num2 = iString(codefile->Vars.idx - 1);

        // ((___Variable___ *)((CodeFile *)Debug->SourceFiles.arr[i])->Vars.arr[i])
        new_code.Append(&new_code, CreateString((char *[]){"\t___Variable___ __", var_name, "_var_info = { .Pointer = malloc(sizeof(void *)), .Size = sizeof(void *) };" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\t___Any___ ", var_name, " = __", var_name, "_var_info.Pointer;", NULL}));
        LineArgs.Destruct(&LineArgs, 1, 1);
        return new_code;
    }

    /* Create an empty pointer on heap, create variable and set info, add to source code info */
    if(line.Contains(&line, "___Any___()")) { // Pointer
        v->Pointer = (void *)malloc(sizeof(void *));
        memset(v->Pointer, '\0', sizeof(void *));
        codefile->Vars.Append(&codefile->Vars, v);
        char *num = iString(__Main__->SourceFiles.idx);
        char *num2 = iString(codefile->Vars.idx - 1);
        new_code.Append(&new_code, CreateString((char *[]){"\t___Variable___ __", var_name, "_var_info = { .Pointer = malloc(sizeof(void *)), .Size = sizeof(void *) };" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\t___Any___ ", var_name, " = __", var_name, "_var_info.Pointer;", NULL}));
        LineArgs.Destruct(&LineArgs, 1, 1);
        return new_code;
    }

    /* Parse ___Any___(...) for heap size, create variable and set info, add to source code info */
    if(line.Contains(&line, "___Any___(")) { // Pointer
        String any = NewString(strdup(LineArgs.arr[0]));
        if(any.data[any.idx] != ')' && any.data[any.idx - 1] == ')') {
            printf("[ - ] Error, Invalid ___Any___ Allocation....!\n");
        }

        for(int i = 0; i < 10; i++)
            any.TrimAt(&any, 0);
        
        any.Trim(&any, ')');
        v->Size = atoi(any.data);
        v->Pointer = (void *)malloc(sizeof(void *) * v->Size);
        memset(v->Pointer, '\0', sizeof(void *) * v->Size);

        codefile->Vars.Append(&codefile->Vars, v);

        any.Destruct(&any);

        codefile->Vars.Append(&codefile->Vars, v);
        char *num = iString(__Main__->SourceFiles.idx);
        char *num2 = iString(codefile->Vars.idx - 1);
        new_code.Append(&new_code, CreateString((char *[]){"\t___Variable___ *__", var_name, "_var_info = { .Pointer = malloc(", any.data, "), .Size = ", any.data, " };" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\t___Any___ ", var_name, " = __", var_name, "_var_info.Pointer;", NULL}));
        new_code.arr[new_code.idx] = NULL;
        LineArgs.Destruct(&LineArgs, 1, 1);
        return new_code;
    }

    LineArgs.Destruct(&LineArgs, 1, 1);
    return ((Array){0});
}