#ifndef _CARGS_H_
#define _CARGS_H_

typedef struct { int argc; char *argv[1]; } ARGS;
extern ARGS __c_args__;

extern far int main(int argc, char *argv[]);

#define NO_C_ARGS   ((unsigned int)-1)

#endif
