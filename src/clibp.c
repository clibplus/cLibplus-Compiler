#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "clibp.h"

cLibp *__Main__;

// void ___type___(void *x) {
//     for(int i = 0; i < __Main__->SourceFiles.idx; i++) {
//         for(int c = 0; c < ((CodeFile *)__Main__->SourceFiles.arr[i])->Vars.idx; c++) {
//             if(((___Variable___ *)((CodeFile *)__Main__->SourceFiles.arr[i])->Vars.arr[i])->Pointer == x)
//                 return ((___Variable___ *)((CodeFile *)__Main__->SourceFiles.arr[i])->Vars.arr[i])->Name;
//         }
//     }
// }

void pretty_p(const char *q, int err, int ff) {
    printf("%s%s\x1b[0m\n", (err ? "\x1b[31m" : "\x1b[32m"), q);
    if(ff) free((char *)q);
}

/*
    FUTURE IMPLEMENTATION:
*/
cLibp *InitCLP(const char *cmd, int debug) {
    cLibp *p = (cLibp *)malloc(sizeof(cLibp));
    if(!p) {
        printf("[ x ] Error, Unable to allocate memory for cLib+ .....!\n");
        exit(0);
    }

    *p = (cLibp){
        .Files          = NewArray(NULL),
        .SourceFiles    = NewArray(NULL),
        .CompileCmd     = NewString(strdup("sudo gcc ")),
        .AutoFree       = 0,
        .CAutoFree      = 0,
        .Debug          = debug
    };
    __Main__ = p;

    if(CheckCmd(p, cmd) < 0) {
        printf("[ - ] Error, Invalid arguments supplied....!\n");
        return NULL;
    }

    Parse_cLibp(p);
    p->CompileCmd.AppendArray(&p->CompileCmd, (const char *[]){"-lclibp -lstr -larr -lmap -lOS", NULL});
    if(p->Debug)
        p->CompileCmd.AppendString(&p->CompileCmd, " -g -g3 -ggdb -ggdb3");

    p->CompileCmd.data[p->CompileCmd.idx] = '\0';
    pretty_p(CreateString((char *[]){"[ cLib+ COMPILATION CMD ]: ", p->CompileCmd.data, "\n[ cLib+ GCC Response ]: ....", NULL}), 0, 1);
    char *t = ExecuteCmd(p->CompileCmd.data);

    return p;
}

int CheckCmd(cLibp *c, const char *cmd) {
    String r = NewString(strdup(cmd));
    Array g = NewArray((const void **)r.Split(&r, " "));
    c->CmdArgs = NewArray((const void **)r.Split(&r, " "));
    
    int found = 0;
    for(int i = 0; i < g.idx; i++) {
        String n = NewString(g.arr[i]);
        if(n.Is(&n, "clp") || n.Is(&n, "./clp") || n.Is(&n, "/root/clp")) {
            c->CmdArgs.Remove(&c->CmdArgs, i);
            found = 1;
        }

        if(n.Is(&n, "--autofree")) {
            c->AutoFree = 1;
            c->CmdArgs.Remove(&c->CmdArgs, i);
        }

        if(n.Contains(&n, ".clp")) {
            c->Files.Append(&c->Files, strdup(n.data));
        }

        (c->Debug ? pretty_p(CreateString((char *[]){"[ cLib+ ARGV(s) ]: ", n.data, NULL}), 0, 1) : 0);

        // if(n.Is(&n, "--c-autofree")) {}
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
        .NewContent = Openfile(CreateString((char *[]){(char *)file, "_clp.c", NULL}), FILE_WRITE_READ)
    };

    if(!c || !file)
        return -1;

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
    for(int i = 0; i < lines.idx; i++) {
        String line = NewString(lines.arr[i]);
        if(line.StartsWith(&line, "int main") && main) {
            main_found = 1;
        }

        if(line.Contains(&line, "___Any___")) {
            Array new_line = __Parse_Any(c, codefile, line);

            if(new_line.idx > 0) {
                char *g = new_line.Join(&new_line, "\n");
                codefile->NewContent.Write(&codefile->NewContent, g);
                codefile->NewContent.Write(&codefile->NewContent, "\n");
                new_line.Destruct(&new_line, 1, 1);
                free(g);
            } else { printf("[ - ] Error, Invalid syntax on line: %d", i); }
            continue;
        }

        if(line.Contains(&line, "___str___")) {
            Array new_line = __Parse_String(c, codefile, line);
            if(new_line.idx > 0) {
                char *g = new_line.Join(&new_line, "\n");
                codefile->NewContent.Write(&codefile->NewContent, g);
                codefile->NewContent.Write(&codefile->NewContent, "\n");
                new_line.Destruct(&new_line, 1, 1);
                free(g);
            } else { printf("[ - ] Error, Invalid syntax on line: %d", i); }
            continue;
        }

        int ye = 0;
        for(int v = 0; v < codefile->Vars.idx; v++) {
            if(line.Contains(&line, ((___Variable___ *)codefile->Vars.arr[v])->Name) && line.Contains(&line, ".Destruct();")) {
                
                int b;
                if((b = line.CountChar(&line, ' ')) > -1)
                    for(int n = 0; n < b; n++)
                        line.Trim(&line, ' ');
                    
                Array args = NewArray((const void **)line.Split(&line, "."));
                char *g = CreateString((char *[]){"\t", ((___Variable___ *)codefile->Vars.arr[v])->TempName, ".Destruct(&", ((___Variable___ *)codefile->Vars.arr[v])->TempName, ");", NULL});
    
                if(g != NULL) {
                    codefile->NewContent.Write(&codefile->NewContent, g);
                    codefile->NewContent.Write(&codefile->NewContent, "\n");
                    free(g);
                    ye = 1;
                    break;
                } else { printf("[ - ] Error, Invalid syntax on line: %d", i); }
            }
        }
        
        if(ye)
            continue;

        codefile->NewContent.Write(&codefile->NewContent, line.data);
        codefile->NewContent.Write(&codefile->NewContent, "\n");

        line.Destruct(&line);
    }

    
    if(c->Debug) {
        char *test = codefile->NewContent.Read(&codefile->NewContent);
        pretty_p(CreateString((char *[]){"[ cLib+ - C Generated Code ]:\n", NULL}), 0, 1);
        pretty_p(CreateString((char *[]){test, "\n", NULL}), 1, 1);
    }

    c->CompileCmd.AppendArray(&c->CompileCmd, (const char *[]){codefile->NewContent.path, " ", NULL});
    codefile->NewContent.Destruct(&codefile->NewContent);
    Content.Destruct(&Content);
    c->SourceFiles.Append(&c->SourceFiles, codefile);
    return 1;
}

Array __Parse_Any(cLibp *c, CodeFile *codefile, String line) {
    ___Variable___ *v = (___Variable___ *)malloc(sizeof(___Variable___));
    *v = (___Variable___){};
    Array new_code = NewArray(NULL);

    int b;
    if((b = line.CountChar(&line, '\t')) > -1)
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
    v->TempName = CreateString((char *[]){"__", var_name, "_var_info", NULL});

    /* Create an empty pointer on heap, create variable and set info, add to source code info */
    if(line.Contains(&line, "___Any___()")) { // Pointer
        v->Pointer = (void *)malloc(sizeof(void *));
        memset(v->Pointer, '\0', sizeof(void *));
        codefile->Vars.Append(&codefile->Vars, v);
        char *num = iString(__Main__->SourceFiles.idx);
        char *num2 = iString(codefile->Vars.idx - 1);
        new_code.Append(&new_code, CreateString((char *[]){"\t___Variable___ __", var_name, "_var_info = { .Pointer = malloc(1024), .Size = 1024, .Destruct = DestructVar };" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\tmemset(__", var_name, "_var_info.Pointer, '\\0', 1024);" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\t___Any___", var_name, " = __", var_name, "_var_info.Pointer;", NULL}));
        LineArgs.Destruct(&LineArgs, 1, 1);
        return new_code;
    }

    /* Parse ___Any___(...) for heap size, create variable and set info, add to source code info */
    if(line.Contains(&line, "___Any___(")) { // Pointer
        String any = NewString(strdup(LineArgs.arr[0]));
        if(any.data[any.idx] != ')' && any.data[any.idx - 1] != ')') {
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
        new_code.Append(&new_code, CreateString((char *[]){"\t___Variable___ __", var_name, "_var_info = { .Pointer = malloc(", any.data, "), .Size = ", any.data, ", .Destruct = DestructVar };" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\tmemset(__", var_name, "_var_info.Pointer, '\\0', ", any.data, ");" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\t___Any___ ", var_name, " = __", var_name, "_var_info.Pointer;", NULL}));
        new_code.arr[new_code.idx] = NULL;
        LineArgs.Destruct(&LineArgs, 1, 1);
        return new_code;
    }

    
    /* Create a pointer as stack usage, create variable and set info, add to source code info */
    if(line.Contains(&line, "___Any___")) { // Stack
        v->Pointer = (void *)malloc(sizeof(void *));
        memset(v->Pointer, '\0', sizeof(void *));
        codefile->Vars.Append(&codefile->Vars, v);
        char *num = iString(__Main__->SourceFiles.idx);
        char *num2 = iString(codefile->Vars.idx - 1);

        // ((___Variable___ *)((CodeFile *)Debug->SourceFiles.arr[i])->Vars.arr[i])
        new_code.Append(&new_code, CreateString((char *[]){"\t___Variable___ __", var_name, "_var_info = { .Pointer = malloc(sizeof(void *)), .Size = sizeof(void *), .Destruct = DestructVar };" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\tmemset(__", var_name, "_var_info.Pointer, '\\0', sizeof(void *));" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\t___Any___ ", var_name, " = __", var_name, "_var_info.Pointer;", NULL}));
        LineArgs.Destruct(&LineArgs, 1, 1);
        return new_code;
    }

    LineArgs.Destruct(&LineArgs, 1, 1);
    return ((Array){0});
}

Array __Parse_String(cLibp *c, CodeFile *codefile, String line) {
    ___Variable___ *v = (___Variable___ *)malloc(sizeof(___Variable___));
    memset(v, '\0', sizeof(___Variable___));

    Array new_code = NewArray(NULL);

    int b;
    if((b = line.CountChar(&line, '\t')) > -1)
        for(int n = 0; n < b; n++)
            line.Trim(&line, '\t');

    Array LineArgs = NewArray((const void **)line.Split(&line, " "));
    if(LineArgs.idx < 2) {
        printf("[ - ] Error, Invalid syntax for ___String___....!\n");
        exit(0);
    }

    char *var_name = LineArgs.arr[1];
    v->Name = strdup(var_name);
    v->TempName = CreateString((char *[]){"__", var_name, "_var_info", NULL});

    /* Create an empty pointer on heap, create variable and set info, add to source code info */
    if(line.Contains(&line, "___str___()")) { // Pointer
        v->Pointer = (void *)malloc(sizeof(void *));
        memset(v->Pointer, '\0', sizeof(void *));
        codefile->Vars.Append(&codefile->Vars, v);
        char *num = iString(__Main__->SourceFiles.idx);
        char *num2 = iString(codefile->Vars.idx - 1);
        new_code.Append(&new_code, CreateString((char *[]){"\tString __", var_name, "_var_info = { .data = malloc(1024), .Size = 1024, .Destruct = DestructVar };\n\tConstructMethods(&__", var_name, ");" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\tmemset(__", var_name, "_var_info.data, '\\0', 1024);" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\t___str___ ", var_name, " = __", var_name, "_var_info.data;", NULL}));
        LineArgs.Destruct(&LineArgs, 1, 1);
        return new_code;
    }

    /* Parse ___Any___(...) for heap size, create variable and set info, add to source code info */
    if(line.Contains(&line, "___str___(")) { // Pointer
        String any = NewString(strdup(LineArgs.arr[0]));
        if(any.data[any.idx] != ')' && any.data[any.idx - 1] != ')') {
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
        new_code.Append(&new_code, CreateString((char *[]){"\tString __", var_name, "_var_info = { .data = malloc(", any.data, "), .idx = ", any.data, " };\n\tConstructMethods(&__", var_name, ");" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\tmemset(__", var_name, "_var_info.Pointer, '\\0', 1024);" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\t___str___ ", var_name, " = __", var_name, "_var_info.data;", NULL}));
        new_code.arr[new_code.idx] = NULL;
        LineArgs.Destruct(&LineArgs, 1, 1);
        return new_code;
    }

    
    /* Create a pointer as stack usage, create variable and set info, add to source code info */
    if(line.Contains(&line, "___str___")) { // Stack
        v->Pointer = (void *)malloc(sizeof(void *));
        memset(v->Pointer, '\0', sizeof(void *));
        codefile->Vars.Append(&codefile->Vars, v);
        char *num = iString(__Main__->SourceFiles.idx);
        char *num2 = iString(codefile->Vars.idx - 1);

        // ((___Variable___ *)((CodeFile *)Debug->SourceFiles.arr[i])->Vars.arr[i])
        new_code.Append(&new_code, CreateString((char *[]){"\tString __", var_name, "_var_info = NewString(NULL);" , NULL}));
        new_code.Append(&new_code, CreateString((char *[]){"\t___str___ ", var_name, " = __", var_name, "_var_info.data;", NULL}));
        LineArgs.Destruct(&LineArgs, 1, 1);
        return new_code;
    }

    LineArgs.Destruct(&LineArgs, 1, 1);
    return ((Array){0});
    
}

void DestructVar(___Variable___ *v) {
    if(v->Type)
        free(v->Type);

    if(v->Name)
        free(v->Name);

    if(v->TempName)
        free(v->TempName);

    if(v->Pointer)
        free(v->Pointer);
}