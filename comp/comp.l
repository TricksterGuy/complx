D [0-9]
H [a-fA-F0-9]
A [A-Za-z]
AN [A-Za-z0-9_]
L [a-zA-Z_]
W [ \t]*

%{
#include <stdio.h>
#include <comp.tab.hpp>
%}

%%
run             return RUN;
step            return STEP;
back            return BACK;
next            return NEXT;
prev            return PREV;
finish          return FINISH;
rewind          return REWIND;

tempbreak       return TEMPBREAK;
break           return BREAK;
watch           return WATCH;
blackbox        return BLACKBOX;
undostack       return UNDOSTACK;
callstack       return CALLSTACK;
delete          return DELETE;

set             return SET;
fillmem         return FILLMEM;
input           return INPUT;
output          return OUTPUT;
reset           return RESET;
randomize       return RANDOMIZE;
truetraps       return TRUETRAPS;
interrupt       return INTERRUPT;

list            return LIST;

load            return LOAD;
reload          return RELOAD;
loadover        return LOADOVER;
reloadover      return RELOADOVER;
quit            return QUIT;
exit            return QUIT;

{W};{W}             return SEP;
{W};?{W}\n          return END;
-?{D}+              yylval.val=atoi(yytext);                   return NUMBER;
[Rr][0-7]           yylval.reg=yytext[1] - '0';                return GPREG;
[Pp][Cc]            yylval.reg=8;                              return REG;
[Cc][Cc]            yylval.reg=9;                              return REG;
x{H}                yylval.addr=strtol(yytext + 1, NULL, 16);  return ADDR;
x{H}{H}             yylval.addr=strtol(yytext + 1, NULL, 16);  return ADDR;
x{H}{H}{H}          yylval.addr=strtol(yytext + 1, NULL, 16);  return ADDR;
x{H}{H}{H}{H}       yylval.addr=strtol(yytext + 1, NULL, 16);  return ADDR;
[^ \t\n]+           yylval.str=yytext;                         return STRING;

%%
