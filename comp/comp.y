%code requires
{
#include <commands.hpp>
#include <lc3_all.hpp>
#include <stdio.h>

typedef union YYSTYPE {
    unsigned char reg;
    unsigned short addr;
    int val;
    char* str;
} YYSTYPE;

#define YYSTYPE union YYSTYPE

int yylex(void);
void yyerror(const char *s);
void warning(const char* s, char* t);

}

%token NUMBER GPREG REG ADDR STRING
%token RUN STEP BACK NEXT PREV FINISH REWIND
%token TEMPBREAK BREAK WATCH BLACKBOX UNDOSTACK CALLSTACK DELETE
%token SET FILLMEM INPUT OUTPUT RESET RANDOMIZE TRUETRAPS INTERRUPT
%token LIST
%token LOAD RELOAD LOADOVER RELOADOVER
%token QUIT END
%left SEP

%type <val> NUMBER
%type <reg> GPREG
%type <reg> REG
%type <addr> ADDR
%type <str> STRING
%define parse.error verbose
%define parse.lac full

%%
statement : command SEP statement | command END | command;
command: ctrl_command | debug_command | manip_command | display_command | file_command | quit_command;
ctrl_command: run_command | step_command | back_command | next_command | prev_command | finish_command | rewind_command;
debug_command: tempbreak_command | break_command | watch_command | blackbox_command | undostack_command | callstack_command | delete_command;
manip_command: set_command | fillmem_command | input_command | output_command | reset_command | randomize_command | truetraps_command | interrupt_command;
display_command: list_command;
file_command: load_command | reload_command | loadover_command | reloadover_command;

run_command: RUN
    {
        do_run();
    }
    ;
step_command: STEP
    {
        do_step();
    }
    | STEP NUMBER
    {
        do_step($2);
    }
    ;
back_command: BACK
    {
        do_back();
    }
    | BACK NUMBER
    {
        do_back($2);
    }
    ;
next_command: NEXT
    {
        do_next();
    }
    | NEXT NUMBER
    {
        do_back($2);
    }
    ;
prev_command: PREV
    {
        do_prev();
    }
    | PREV NUMBER
    {
        do_prev($2);
    }
    ;
finish_command: FINISH
    {
        do_finish();
    }
    ;
rewind_command: REWIND
    {
        do_rewind();
    }
    ;
tempbreak_command: TEMPBREAK ADDR | TEMPBREAK STRING
    {
        do_tempbreak($2);
    }
    ;
break_command: BREAK break_args;
break_args: STRING STRING STRING NUMBER | ADDR STRING STRING NUMBER
    {
        do_break($1, $2, $3, $4);
    }
    | STRING STRING STRING | ADDR STRING STRING
    {
        do_break($1, $2, $3);
    }
    | STRING STRING | ADDR STRING
    {
        do_break($1, $2);
    }
    | STRING | ADDR
    {
        do_break($1);
    }
    ;
watch_command: WATCH watch_args;
watch_args: GPREG STRING STRING NUMBER | ADDR STRING STRING NUMBER | STRING STRING STRING NUMBER
    {
        do_watch($1, $2, $3, $4);
    }
    | GPREG STRING STRING | ADDR STRING STRING | STRING STRING STRING
    {
        do_watch($1, $2, $3);
    }
    | GPREG STRING | ADDR STRING | STRING STRING
    {
        do_watch($1, $2);
    }
    ;
blackbox_command: BLACKBOX blackbox_args
blackbox_args: ADDR STRING STRING | STRING STRING STRING
    {
        do_blackbox($1, $2, $3);
    }
    | ADDR STRING | STRING STRING
    {
        do_blackbox($1, $2);
    }
    | ADDR | STRING
    {
        do_blackbox($1);
    }
    ;
undostack_command: UNDOSTACK NUMBER
    {
        do_undostack($2);
    }
    ;
callstack_command: CALLSTACK NUMBER
    {
        do_callstack($2);
    }
    ;
delete_command: DELETE GPREG | DELETE ADDR | DELETE STRING
    {
        do_delete($2);
    }
    ;
set_command: SET set_args;
set_args: REG NUMBER | GPREG NUMBER
    {
        do_set((unsigned char)$1, $2);
    }
    | REG ADDR | GPREG ADDR
    {
        do_set((unsigned char)$1, $2);
    }
    | GPREG STRING | REG STRING
    {
        do_set((unsigned char)$1, $2);
    }
    | ADDR NUMBER
    {
        do_set((unsigned short)$1, $2);
    }
    | ADDR ADDR
    {
        do_set((unsigned short)$1, $2);
    }
    | ADDR STRING
    {
        do_set((unsigned short)$1, $2);
    }
    | STRING NUMBER
    {
        do_set($1, $2);
    }
    | STRING ADDR
    {
        do_set($1, $2);
    }
    | STRING STRING
    {
        do_set($1, $2);
    }
    ;
fillmem_command: FILLMEM NUMBER
    {
        do_fillmem($2);
    }
    ;
input_command: INPUT STRING
    {
        do_input($2);
    }
    ;
output_command: OUTPUT STRING
    {
        do_output($2);
    }
    ;
reset_command: RESET
    {
        do_reset();
    }
    ;
randomize_command: RANDOMIZE
    {
        do_randomize();
    }
    ;
truetraps_command: TRUETRAPS NUMBER
    {
        do_truetraps($2);
    }
    ;
interrupt_command: INTERRUPT NUMBER
    {
        do_interrupt($2);
    }
    ;
list_command: LIST list_args;
list_args: ADDR NUMBER
    {
        do_list($1, $2);
    }
    | ADDR
    {
        do_list($1);
    }
    | STRING NUMBER
    {
        do_list($1, $2);
    }
    | STRING
    {
        do_list($1);
    }
    ;
load_command: LOAD STRING
    {
        do_load($2);
    }
    ;
reload_command: RELOAD
    {
        do_reload();
    }
    ;
loadover_command: LOADOVER STRING
    {
        do_loadover($2);
    }
    ;
reloadover_command: RELOADOVER
    {
        do_reloadover();
    }
    ;
quit_command : QUIT
    {
        do_quit();
    }
    ;
%%

int yyterminate()
{
    static bool term = false;
    return (term = !term) ? END : 0;
}
