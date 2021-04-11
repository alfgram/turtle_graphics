#include "../Neill_files/neillsimplescreen.h"
#include "../Neill_files/neillsdl2.h"
#include "../Stack/stack.h"

#define DEG360 360.0
#define DEG180 180.0
#define RGBMAX 255
#define ALPHA 26
#define INIT_X_COORD 400
#define INIT_Y_COORD 300
#define MAXLINELEN 200
#define MAXTOKENLEN 20
#define MAXNUMTOKENS 500

#define str_same(W1, W2) (strcmp(W1, W2) == 0)
/* Converts N in degrees to radians */
#define deg_to_rad(N) (N * M_PI / DEG180)

enum error_no
{
   WRONG_DIRECTION,
   WRONG_START,
   WRONG_SET,
   WRONG_DO,
   WRONG_INSTRUCTION,
   WRONG_BRACKET,
   WRONG_POLISH,
   VAR_NOT_SET
};
typedef enum error_no error_no;

struct position
{
   float x;
   float y;
};
typedef struct position position;

struct variable
{
   /* Set to true if variable has been set */
   bool is_set;
   float val;
};
typedef struct variable variable;

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
   /* Used for storing variable index values,
   variable values, numbers and the result of polish sums */
   float result;
   /* Stores variable values, indexed
   by taking ASCII value 'A'(65) from char value */
   variable var[ALPHA];
   /* Current rotation of curser in degrees */
   float rot;
   /* Current coordinates of curser */
   position coords;
   /* Used for creating SDL window and graphics */
   SDL_Simplewin sw;
};
typedef struct prog prog;

bool main_prog(prog* p);
bool instruction(prog* p);
bool instructlst(prog* p);
bool op(prog* p);
/* Updates coordinates and draws line
based on current rotation and given distance */
bool fd(prog* p);
/* Updates rotation based on a left turn of given degrees */
bool lt(prog* p);
/* Updates rotation based on a right turn of given degrees */
bool rt(prog* p);
/* Verifies word is valid number or variable
and stores the value in p->result */
bool varnum(prog* p);
/* Verifies word is valid variable
and stores the variable index in p->result */
bool var(prog* p);
/* Repeats contents of loop incrimenting/decrimenting
variable value until it reaches 2nd number */
bool do_loop(prog* p);
/* Sets variable value to result of polish calculation */
bool set(prog* p);
/* Calculates polish sum and stores result in in p->result */
bool polish(prog* p, stack* s);
/* Used for polish calculations.
Based on infix calculator provided by Neill */
bool calc_polish(prog* p, stack* s);
/* Updates coordinates based on current rotation
and given distance using trigonometry */
void update_coords(prog* p);
/* Repeats contents of do loop incrimienting or decrimenting
variable value untill it reaches 2nd number value */
bool repeat_do_loop(prog* p, int var_index);

void init_SDL(prog* p);
void run_SDL(prog* p);
float mod_360(float num);
void print_error(prog* p, char* filename);
void print_error_message(prog* p);
void load_file(prog* p, char* filename);

void test(void);
