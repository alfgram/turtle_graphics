#include "../Neill_files/general.h"

typedef float stacktype;

typedef struct stack stack;

/* Create an empty stack */
stack* stack_init(void);
/* Add element to top */
void stack_push(stack* s, stacktype i);
/* Take element from top */
bool stack_pop(stack* s, stacktype* d);
/* Clears all space used */
bool stack_free(stack* s);

/* Copy top element into d (but don't pop it) */
bool stack_peek(stack*s,  stacktype* d);
