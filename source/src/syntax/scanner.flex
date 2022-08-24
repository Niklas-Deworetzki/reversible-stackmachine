%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "syntax/scanner.h"
#include "parser.h"

%}

%option yylineno

%option noyywrap
%option never-interactive
%option nounistd

%option nounput
%option noinput

%{

    static int symbol(int type) {
        return type;
    }

    static int symbol_string(int type, char* value, size_t length) {
        yylval.string = malloc(length + 1);
        memcpy(yylval.string, value, length);
        yylval.string[length] = 0;
        return type;
    }

    static int symbol_integer(int type, int value) {
        yylval.number = value;
        return type;
    }

    static int symbol_instruction(size_t offset, bool is_forward) {
        yylval.instruction.offset = offset;
        yylval.instruction.is_forward = is_forward;
        return INSTRUCTION;
    }

    static int symbol_unknown(int type, const char *description, char *lexeme) {
        fprintf(stderr, "Encountered '%s' which is not a known %s.\n", lexeme, description);
        return type;
    }
%}


identifier  [A-Za-z_][A-Za-z0-9_.]*
label       {identifier}:
directive   "."{identifier}
number      [+-]?[0-9]+
hexnumber   0x[0-9a-fA-F]+

whitespace  [ \t\f]+
linebreak   [(\r\n)\r\n]
linecomment ;.*

%%
yyleng


{whitespace}    { /* Ignore whitespace */ }
{linecomment}   { /* Ignore comments */ }

{linebreak}     { return symbol(LINEBREAK); }

"["             { return symbol(LEFT); }
"]"             { return symbol(RIGHT); }
"@"             { return symbol(AT); }
"+"             { return symbol(PLUS); }
"-"             { return symbol(MINUS); }

".word"             { return symbol(DOT_WORD); }
".set"              { return symbol(DOT_SET); }
".bss"              { return symbol(DOT_RESERVED); }

"start"               { return symbol_instruction(  0,  true ); }
"stop"                { return symbol_instruction(  0, false ); }
"nop"                 { return symbol_instruction(  1,  true ); }
"pushc"               { return symbol_instruction(  2,  true ); }
"popc"                { return symbol_instruction(  2, false ); }
"dup"                 { return symbol_instruction(  3,  true ); }
"undup"               { return symbol_instruction(  3, false ); }
"swap"                { return symbol_instruction(  4,  true ); }
"bury"                { return symbol_instruction(  5,  true ); }
"dig"                 { return symbol_instruction(  5, false ); }
"allocpar"            { return symbol_instruction(  6,  true ); }
"releasepar"          { return symbol_instruction(  6, false ); }
"asf"                 { return symbol_instruction(  7,  true ); }
"rsf"                 { return symbol_instruction(  7, false ); }
"pushl"               { return symbol_instruction(  8,  true ); }
"popl"                { return symbol_instruction(  8, false ); }
"call"                { return symbol_instruction(  9,  true ); }
"uncall"              { return symbol_instruction( 10,  true ); }
"branch"              { return symbol_instruction( 11,  true ); }
"brt"                 { return symbol_instruction( 12,  true ); }
"brf"                 { return symbol_instruction( 13,  true ); }
"pushtrue"            { return symbol_instruction( 14,  true ); }
"poptrue"             { return symbol_instruction( 14, false ); }
"pushfalse"           { return symbol_instruction( 15,  true ); }
"popfalse"            { return symbol_instruction( 15, false ); }
"cmpusheq"            { return symbol_instruction( 16,  true ); }
"cmpopeq"             { return symbol_instruction( 16, false ); }
"cmpushne"            { return symbol_instruction( 17,  true ); }
"cmpopne"             { return symbol_instruction( 17, false ); }
"cmpushlt"            { return symbol_instruction( 18,  true ); }
"cmpoplt"             { return symbol_instruction( 18, false ); }
"cmpushle"            { return symbol_instruction( 19,  true ); }
"cmpople"             { return symbol_instruction( 19, false ); }
"inc"                 { return symbol_instruction( 20,  true ); }
"dec"                 { return symbol_instruction( 20, false ); }
"neg"                 { return symbol_instruction( 21,  true ); }
"add"                 { return symbol_instruction( 22,  true ); }
"sub"                 { return symbol_instruction( 22, false ); }
"xor"                 { return symbol_instruction( 23,  true ); }
"shl"                 { return symbol_instruction( 24,  true ); }
"shr"                 { return symbol_instruction( 24, false ); }
"arpushadd"           { return symbol_instruction( 25,  true ); }
"arpopadd"            { return symbol_instruction( 25, false ); }
"arpushsub"           { return symbol_instruction( 26,  true ); }
"arpopsub"            { return symbol_instruction( 26, false ); }
"arpushmul"           { return symbol_instruction( 27,  true ); }
"arpopmul"            { return symbol_instruction( 27, false ); }
"arpushdiv"           { return symbol_instruction( 28,  true ); }
"arpopdiv"            { return symbol_instruction( 28, false ); }
"arpushmod"           { return symbol_instruction( 29,  true ); }
"arpopmod"            { return symbol_instruction( 29, false ); }
"arpushand"           { return symbol_instruction( 30,  true ); }
"arpopand"            { return symbol_instruction( 30, false ); }
"arpushor"            { return symbol_instruction( 31,  true ); }
"arpopor"             { return symbol_instruction( 31, false ); }
"pushm"               { return symbol_instruction( 32,  true ); }
"popm"                { return symbol_instruction( 32, false ); }
"load"                { return symbol_instruction( 33,  true ); }
"store"               { return symbol_instruction( 33, false ); }
"memswap"             { return symbol_instruction( 34,  true ); }
"xorhc"               { return symbol_instruction( 35,  true ); }


{identifier}    { return symbol_string(IDENTIFIER, yytext, yyleng); }
{label}         { return symbol_string(LABEL, yytext, yyleng - 1); }
{number}        { return symbol_integer(NUMBER, strtol(yytext, NULL, 10)); }
{hexnumber}     { return symbol_integer(NUMBER, strtol(yytext + 2, NULL, 16)); }

{directive}     { return symbol_unknown(DIRECTIVE, "directive", yytext); }
.               { return symbol(YYUNDEF); }

%%
