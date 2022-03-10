#pragma once

#include <stdio.h> // NOLINT(modernize-deprecated-headers) <-- scanner is compiled as C file, so stdio.h is required.

extern FILE *yyin;
extern char *yytext;

int yylex();
