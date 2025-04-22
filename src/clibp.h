#pragma once


#include <str.h>
#include <arr.h>
#include <map.h>
#include <OS/file.h>
#include <OS/utils.h>

#include <___utils___.h>

typedef void *___Any___;
typedef char *___str___;

typedef struct ___Struct___ {
    char *NAME;
} ___Struct___;

typedef struct ___Variable___ {
    void    *Type;
    char    *Name;
    char    *TempName;
    size_t   Size;
    void    *Pointer;
    int     Initialized;
    void    (*Destruct) (struct ___Variable___ *v);
} ___Variable___;

typedef Array VarList;
typedef Array StructList;
typedef Array SourceCode;

typedef struct CodeFile {
    String          Filepath;
    File            Content;
    File            NewContent;
    SourceCode      Lines;
    StructList      Structs;
    VarList         Vars;
} CodeFile;

typedef Array CodeFiles;

typedef struct cLibp {
    Array           CmdArgs;
    Array           Files;
    CodeFiles       SourceFiles;

    String          CompileCmd;
    int             AutoFree;
    int             CAutoFree;
    int             Debug;
} cLibp;

#define CLIBP_BUILD_DIR "clibp_build"

extern cLibp *__Main__;

cLibp       *InitCLP(const char *cmd, int debug);
int         CheckCmd(cLibp *c, const char *cmd);
void        Parse_cLibp(cLibp *c);
int         __Parse_File(cLibp *c, const char *file, int main);
Array       __Parse_Any(cLibp *c, CodeFile *codefile, String line);
Array       __Parse_String(cLibp *c, CodeFile *codefile, String line);

void        DestructVar(___Variable___ *v);


// String
String 		ConstructMethods(String *s);