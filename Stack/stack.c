#include "stack.h"
#include "specific.h"

stack* stack_init(void)
{
   stack* s = (stack*) ncalloc(sizeof(stack), 1);
   return s;
}

void stack_push(stack* s, stacktype d)
{
   dataframe* f;
   if(s){
      f = ncalloc(sizeof(dataframe), 1);
      f->i = d;
      f->next = s->start;
      s->start = f;
      s->size = s->size + 1;
   }

}

bool stack_pop(stack* s, stacktype* d)
{
   dataframe* f;
   if((s==NULL) || (s->start==NULL)){
      return false;
   }

   f = s->start->next;
   *d = s->start->i;
   free(s->start);
   s->start = f;
   s->size = s->size - 1;
   return true;
}

bool stack_peek(stack* s, stacktype* d)
{
   if((s==NULL) || (s->start==NULL)){
      return false;
   }
   *d = s->start->i;
   return true;
}

void stack_tostring(stack* s, char* str)
{
   dataframe *p;
   char tmp[ELEMSIZE];
   str[0] = '\0';
   if((s==NULL) || (s->size <1)){
      return;
   }
   p = s->start;
   while(p){
      sprintf(tmp, FORMATSTR, p->i);
      strcat(str, tmp);
      strcat(str, "|");
      p = p->next;
   }
   str[strlen(str)-1] = '\0';
}

bool stack_free(stack* s)
{
   if(s){
      dataframe* tmp;
      dataframe* p = s->start;
      while(p!=NULL){
         tmp = p->next;
         free(p);
         p = tmp;
      }
      free(s);
   }
   return true;
}
