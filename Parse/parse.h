#include "../Neill_files/general.h"
#include "../Neill_files/neillsimplescreen.h"

#define MAXLINELEN 200
#define MAXNUMTOKENS 500
#define MAXTOKENLEN 20
#define str_same(W1, W2) (strcmp(W1, W2) == 0)

enum error_no
{
   WRONG_DIRECTION,
   WRONG_START,
   WRONG_SET,
   WRONG_DO,
   WRONG_INSTRUCTION,
   WRONG_BRACKET
};
typedef enum error_no error_no;

struct word
{
   char word[MAXTOKENLEN];
   /* Holds line number in which
   word was taken from the .ttl file */
   int line_no;
};
typedef struct word word;

struct prog
{
   word wds[MAXNUMTOKENS];
   /* Current word number */
   int cw;
   error_no err;
};
typedef struct prog prog;

void test(void);

/* Prints user's .ttl file to the terminal
with line with error on highlighted in red */
void print_error(prog* p, char* filename);
/* Prints corresponding .err message
also stating which line the error occoured */
void print_error_message(prog* p);
void load_file(prog* p, char* filename);

/*
<MAIN_PROG> ::= "{" <INSTRCTLST>
<INSTRCTLST> ::= <INSTRUCTION><INSTRCTLST> | "}"
<INSTRUCTION> ::= <FD> | <LT> | <RT> | <DO> | <SET>
<FD> ::= "FD" <VARNUM>
<LT> ::= "LT" <VARNUM>
<RT> ::= "RT" <VARNUM>
<DO> ::= "DO" <VAR> "FROM" <VARNUM> "TO"
<VARNUM> "{" <INSTRCTLST>
<VAR> ::= [A−Z]
<VARNUM> ::= number | <VAR>
<SET> ::= "SET" <VAR> ":=" <POLISH>
<POLISH> ::= <OP> <POLISH> | <VARNUM> <POLISH> | ";"
<OP> ::= "+" | "−" | "*" | "/"
*/

/* The functions below verify program p follows the
above grammar */
bool main_prog(prog* p);
bool instruction(prog* p);
bool instructlst(prog* p);
bool varnum(prog* p);
bool var(prog* p);
bool op(prog* p);
bool polish(prog* p);
bool do_loop(prog* p);
bool set(prog* p);
