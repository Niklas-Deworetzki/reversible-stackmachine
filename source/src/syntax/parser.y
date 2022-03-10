%{
// Allow larger programs to be parsed.
#define YYMAXDEPTH 1000000

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "syntax/csyntax.h"
#include "syntax/scanner.h"

void yyerror(ParsedProgram* program, unsigned int *detected_errors, const char *msg);

extern int yylineno; // Use internal counter for errors
static int linenumber = 1; // and count an extra one for parsed Line instances.

static void preprendList(Line line);

static void setupParser();

static void finishProgram(ParsedProgram* program);

%}

%code requires{
    struct instruction_descriptor {
        size_t  offset;
        bool    is_forward;
    };

    #include "syntax/csyntax.h"
}

%token-table
%parse-param {ParsedProgram* program}
%parse-param {unsigned int *detected_errors}

%define parse.error verbose
%define parse.lac full

%union {
    int32_t number;
    char *  string;
    struct instruction_descriptor instruction;

    Operand          operand;
    PrimitiveOperand primitive;

    OperandList      *operands;
    StringList       *strings;

    Line             line;
    LineList *       lines;
}

%token  PLUS        "+"
        MINUS       "-"
        AT          "relative operand"
        LEFT        "["
        RIGHT       "]"
        LINEBREAK   "end of line"
        DOT_WORD    ".word"
        DOT_SET     ".set"
        DOT_RESERVED    ".reserved_word"
        DIRECTIVE   "unknown directive"

%token <number> NUMBER      "number"
%token <string> LABEL       "label"
%token <string> IDENTIFIER  "identifier"
%token <instruction> INSTRUCTION "instruction"

%type program file lines
%type <line>    line
%type <line>    instruction reserved_words words set_value any_line

%type <strings>  labels
%type <operands> operands

%type <operand> operand
%type <primitive> primitive

%type endofline



%start program

%%

// endofline ::= LINEBREAK +

endofline : LINEBREAK endofline
            { linenumber++; }
          | LINEBREAK
            { linenumber++; }
          ;

// labels ::= (LABEL (endofline LABEL)*)? endofline?

labels : LABEL endofline labels
            { $$ = prependString($1, $3); }
       | LABEL
            { $$ = prependString($1, emptyStringList()); }
       |
            { $$ = emptyStringList(); }
       ;



primitive : IDENTIFIER
            { $$ = mkSymbolic($1); }
          | NUMBER
            { $$ = mkConstant($1); }
          | AT NUMBER
            { $$ = mkRelative($2); }
          | AT PLUS NUMBER
            { $$ = mkRelative($3); }
          | AT MINUS NUMBER
            { $$ = mkRelative(- $3); }
          | AT
            { $$ = mkRelative(0); }
          ;

operand : primitive
            { $$ = mkPrimitive($1); }
        | LEFT primitive RIGHT
            { $$ = mkPrimitive($2); }
        | LEFT primitive PLUS primitive RIGHT
            { $$ = mkAdd($2, $4); }
        | LEFT primitive MINUS primitive RIGHT
            { $$ = mkSub($2, $4); }
        ;


operands : operand operands
            { $$ = prependOperand($1, $2); }
         |
            { $$ = emptyOperandList(); }
         ;



instruction : INSTRUCTION
                { $$ = mkInstruction($1.offset, $1.is_forward, noOperand()); }
            | INSTRUCTION operand
                { $$ = mkInstruction($1.offset, $1.is_forward, $2); }
            ;

reserved_words : DOT_RESERVED operand
                  { $$ = mkReservedWords($2); }
               ;

words : DOT_WORD operands
        { $$ = mkWords($2); }
      ;

set_value : DOT_SET operand operand
            { $$ = mkSetValue($2, $3); }
          ;

any_line : instruction
         | reserved_words
         | words
         | set_value
         ;

line : labels any_line
        { $2.labels = $1; $2.linenumber = linenumber; $$ = $2; }
     ;



lines : line endofline lines
        { preprendList($1); }
      | error endofline lines
      | line
        { preprendList($1); }
      |
        {  }
      ;

file : endofline lines
     | lines
     ;

program : { setupParser(); } file { if (yynerrs > 0) {
                                        YYERROR;
                                     }
                                     finishProgram(program);
                                   }

%%

void yyerror(ParsedProgram* program, unsigned int *detected_errors, const char *msg) {
    (void)program;
    // Remove the leading "syntax error, " from message, if there is more to the message.
    const char *stripped_msg = msg[strlen("syntax error")] == 0 ? msg : msg + strlen("syntax error, ");
    fprintf(stderr, "[ERROR] Line %d: %s\n", yylineno, stripped_msg);
    (*detected_errors)++;
}


// Static buffers for different program sections.
static LineList *code, *data, *bss;

static void setupParser() {
    code = emptyLineList();
    data = emptyLineList();
    bss = emptyLineList();
}

static void finishProgram(ParsedProgram* program) {
    program->code = code;
    program->data = data;
    program->bss = bss;
}

static void preprendList(Line line) {
    LineList **list;
    switch (line.variant) {
        case LINE_RESERVED:
            list = &bss;
            break;

        case LINE_WORDS:
            list = &data;
            break;

        case LINE_SET:
            list = &data;
            break;

        default:
            list = &code;
            break;
    }
    *list = prependLine(line, *list);
}