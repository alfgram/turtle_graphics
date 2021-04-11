#include "interp.h"

int main(int argc, char* argv[])
{
   prog program = {0};
   char* filename = argv[1];
   if (argc != 2) {
      on_error("Please enter filename as command line argument.");
   }
   test();

   init_SDL(&program);

   load_file(&program, filename);
   if (main_prog(&program)) {
      print_error(&program, filename);
      return 1;
   }
   run_SDL(&program);
   return 0;
}

void init_SDL(prog* p)
{
   Neill_SDL_Init(&p->sw);
   Neill_SDL_SetDrawColour(&p->sw, RGBMAX, RGBMAX, RGBMAX);
   p->coords.x = INIT_X_COORD;
   p->coords.y = INIT_Y_COORD;
}

void run_SDL(prog* p)
{
   do
   {
      Neill_SDL_Events(&p->sw);
   } while (!p->sw.finished);
   SDL_Quit();
   atexit(SDL_Quit);
}

bool fd(prog* p)
{
   float prev_x, prev_y;
   if (varnum(p)) {
      return 1;
   }
   prev_x = p->coords.x;
   prev_y = p->coords.y;
   update_coords(p);
   /* Draws line from previous coords to new updated coords */
   SDL_RenderDrawLine(p->sw.renderer, (int)prev_x , (int)prev_y,
   (int)p->coords.x, (int)p->coords.y);

   Neill_SDL_UpdateScreen(&p->sw);
   return 0;
}

void update_coords(prog* p)
{
   p->coords.x = p->coords.x +
   (p->result * (sin(deg_to_rad(p->rot))));

   p->coords.y = p->coords.y +
   (p->result * (cos(deg_to_rad(p->rot))));
}

float mod_360(float num)
{
   if (num <= DEG360) {
      return num;
   }
   return num - DEG360;
}

bool lt(prog* p)
{
   if (varnum(p)) {
      return 1;
   }
   p->rot = mod_360(p->rot + (DEG360 - p->result));
   return 0;
}

bool rt(prog* p)
{
   if (varnum(p)) {
      return 1;
   }
   p->rot = mod_360(p->rot + p->result);
   return 0;
}

bool set(prog* p)
{
   int var_index;
   stack* s;
   p->cw++;
   if (var(p)) {
      p->err = WRONG_SET;
      return 1;
   }
   var_index = p->result;
   p->cw++;
   if (!str_same(p->wds[p->cw].word, ":=")) {
      p->err = WRONG_SET;
      return 1;
   }
   p->cw++;
   s = stack_init();
   if (polish(p, s)) {
      stack_free(s);
      p->err = WRONG_POLISH;
      return 1;
   }
   /* Variable value at var_index
   is set to the result of polish calculation */
   p->var[var_index].is_set = true;
   p->var[var_index].val = p->result;
   stack_free(s);
   return 0;
}

bool do_loop(prog* p)
{
   int var_index;
   p->cw++;
   if (var(p)) {
      p->err = WRONG_DO;
      return 1;
   }
   var_index = (int)p->result;
   p->cw++;
   if (!str_same(p->wds[p->cw].word, "FROM")) {
      p->err = WRONG_DO;
      return 1;
   }
   p->cw++;
   if (varnum(p)) {
      p->err = WRONG_DO;
      return 1;
   }
   /* Variable value at var_index
   is set to value of result from varnum */
   p->var[var_index].is_set = true;
   p->var[var_index].val = p->result;
   p->cw++;
   if (!str_same(p->wds[p->cw].word, "TO")) {
      p->err = WRONG_DO;
      return 1;
   }
   p->cw++;
   if (varnum(p)) {
      p->err = WRONG_DO;
      return 1;
   }
   p->cw++;
   if (!str_same(p->wds[p->cw].word, "{")) {
      p->err = WRONG_DO;
      return 1;
   }
   if (repeat_do_loop(p, var_index)){
      return 1;
   }
   return instructlst(p);
}

bool repeat_do_loop(prog* p, int var_index)
{
   /* Saves original current word
   so it can be looped back to */
   int orig_cw = p->cw;
   float end_num = p->result;
   /* If variable is greater than 2nd num
   take 1 from variable in loop */
   if (p->var[var_index].val > end_num) {
      while (p->var[var_index].val-- > end_num)
      {
         p->cw = orig_cw;
         if (instructlst(p)) {
            return 1;
         }
      }
   }
   /* If variable is less than 2nd num
   add 1 to variable in loop */
   else {
      while (p->var[var_index].val++ < end_num)
      {
         p->cw = orig_cw;
         if (instructlst(p)) {
            return 1;
         }
      }
   }
   p->cw = orig_cw;
   return 0;
}

bool instruction(prog* p)
{
   if (str_same(p->wds[p->cw].word, "DO")) {
      return do_loop(p);
   }
   if (str_same(p->wds[p->cw].word, "SET")) {
      return set(p);
   }
   if (str_same(p->wds[p->cw].word, "FD")) {
      p->cw++;
      return fd(p);
   }
   if (str_same(p->wds[p->cw].word, "LT")) {
      p->cw++;
      return lt(p);
   }
   if (str_same(p->wds[p->cw].word, "RT")) {
      p->cw++;
      return rt(p);
   }
   p->err = WRONG_INSTRUCTION;
   return 1;
}

bool instructlst(prog* p)
{
   p->cw++;
   /* If end of instructlst is reached
   without closing bracket */
   if (p->wds[p->cw].word[0] == '\0') {
      p->err = WRONG_BRACKET;
      return 1;
   }
   if (str_same(p->wds[p->cw].word, "}")) {
      return 0;
   }
   if (instruction(p)) {
      return 1;
   }
   return instructlst(p);
}

bool var(prog* p)
{
   if (strlen(p->wds[p->cw].word) == 1 &&
   p->wds[p->cw].word[0] >= 'A' &&
   p->wds[p->cw].word[0] <= 'Z') {
      p->result = p->wds[p->cw].word[0] - 'A';
      return 0;
   }
   return 1;
}

bool varnum(prog* p)
{
   float num;
   int len;
   /* Check if string contains a number, compare
   number of chars read in with strlen of word
   to ensure the string contains no non numeric chars */
   if (sscanf(p->wds[p->cw].word, "%f%n", &num, &len) == 1 &&
   len == (int)strlen(p->wds[p->cw].word)) {
      p->result = num;
      return 0;
   }
   if (!var(p)) {
      /* Check if attempting to use an unset variable */
      if (p->var[(int)p->result].is_set == false) {
         p->err = VAR_NOT_SET;
         return 1;
      }
      p->result = p->var[(int)p->result].val;
      return 0;
   }
   p->err = WRONG_DIRECTION;
   return 1;
}

bool op(prog* p)
{
   if (str_same(p->wds[p->cw].word, "+") ||
   str_same(p->wds[p->cw].word, "-") ||
   str_same(p->wds[p->cw].word, "*") ||
   str_same(p->wds[p->cw].word, "/")) {
      return 0;
   }
   return 1;
}

bool main_prog(prog* p)
{
   if (!str_same(p->wds[p->cw].word, "{")) {
      p->err = WRONG_START;
      return 1;
   }
   return instructlst(p);
}

void load_file(prog* p, char* filename)
{
   char line_buf[MAXLINELEN], *word_p;
   int word_cnt = 0, line_no = 1, offset;
   FILE* fp = (FILE*)nfopen(filename, "r");

   /* File is read line by line then, words are then extracted
   so the line number of each word can be retained */
   while (fgets(line_buf, MAXLINELEN, fp) != NULL)
   {
      /* Num of chars read (offset) is recorded
      so word_p can move step through string */
      offset = 0;
      word_p = line_buf;
      while (sscanf(word_p += offset,
      "%s%n", p->wds[word_cnt].word, &offset) == 1)
      {
         if (word_cnt > MAXNUMTOKENS) {
            on_error(".ttl file too long\n");
         }
         p->wds[word_cnt++].line_no = line_no;
      }

      line_no++;
   }
   fclose(fp);
}

void print_error(prog* p, char* filename)
{
   int line_no = 1;
   char buf[MAXLINELEN];
   FILE* fp = (FILE*)nfopen(filename, "r");
   while (fgets(buf, MAXLINELEN, fp) != NULL)
   {
      /* If error is present on line print line in red */
      if (line_no == p->wds[p->cw].line_no) {
         neillfgcol(red);
         printf("%2d| %s", line_no, buf);
         neillfgcol(white);
      }
      else {
         printf("%2d| %s", line_no, buf);
      }
      line_no++;
   }
   fclose(fp);
   print_error_message(p);
}

void print_error_message(prog* p)
{
   neillfgcol(red);
   printf("error: ");
   neillfgcol(white);
   switch (p->err)
   {
      case WRONG_INSTRUCTION:
         printf("Invalid instruction on line %d.\n"
         "Instructions can only start with the following words: "
         "SET | DO | FT | LT | RT\n", p->wds[p->cw].line_no);
         break;
      case WRONG_DIRECTION:
         printf("Invalid directional command on line %d.\n"
         "Directionional commands (FD | LT | RT) "
         "should be followed by a valid number or variable.\n",
         p->wds[p->cw].line_no);
         break;
      case WRONG_START:
         printf("Missing opening bracket.\n"
         "All .ttl files should start with {\n");
         break;
      case WRONG_DO:
         printf("Incorrect DO command on line %d.\n"
         "DO commands should have the format: "
         "DO <variable> FROM <number/variable> TO <number/variable> {\n",
         p->wds[p->cw].line_no);
         break;
      case WRONG_SET:
         printf("Incorrect SET command on line %d.\n"
         "SET commands should have the format: "
         "SET <variable> := <expression> "
         "where expression is in polish notation.\n",
         p->wds[p->cw].line_no);
         break;
      case WRONG_POLISH:
         printf("Incorrect inverse polish notation on line %d.\n"
         "Ensure correct number of operands / numbers / variables.\n",
         p->wds[p->cw].line_no);
         break;
      case WRONG_BRACKET:
         printf("Expected closing bracket.\n");
         break;
      case VAR_NOT_SET:
         printf("Attempting to use uninitialized variable %s on line %d\n",
         p->wds[p->cw].word, p->wds[p->cw].line_no);
         break;
   }
}

bool calc_polish(prog* p, stack* s)
{
   float n1, n2;
   /* If number or varible push result */
   if (!varnum(p)) {
      stack_push(s, p->result);
   }
   /* If operand pop 2 off stack and perform operation
   push result back onto stack */
   else {
      /* If there is less than 2 numbers on stack give error */
      if (!stack_pop(s, &n2) || !stack_pop(s, &n1)) {
         return 1;
      }
      switch (p->wds[p->cw].word[0])
      {
         case '+' :
            p->result = n1 + n2;
            break;
         case '-' :
            p->result = n1 - n2;
            break;
         case '*' :
            p->result = n1 * n2;
            break;
         case '/' :
            p->result = n1 / n2;
            break;
      }
      stack_push(s, p->result);
   }
   return 0;
}

bool polish(prog* p, stack* s)
{
   float test;
   if (str_same(p->wds[p->cw].word, ";")) {
      stack_pop(s, &p->result);
      /* Check if stack is empty,
      if not throw error */
      if (stack_peek(s, &test)) {
         return 1;
      }
      return 0;
   }
   if (op(p) && varnum(p)) {
      return 1;
   }
   if (calc_polish(p, s)) {
      return 1;
   }
   p->cw++;
   if (polish(p, s)) {
      return 1;
   }
   return 0;
}

void test(void)
{
   stack* s;
   prog empty = {0};
   prog test_files, general, test, test1, test_main_prog, test_op, test_var, polish_test, test_do_loop = {0}, test_set, test_calc_polish = {0};

   /* Test mod_360 */
   assert(fabs(mod_360(180.0) - 180.0) < 0.01);
   assert(fabs(mod_360(97.1) - 97.1) < 0.01);
   assert(fabs(mod_360(400.0) - 40.0) < 0.01);
   assert(fabs(mod_360(1080.5) - 720.5) < 0.01);


   /* Test calc_polish */
   s = stack_init();
   strcpy(test_calc_polish.wds[0].word, "5");
   strcpy(test_calc_polish.wds[1].word, "5");
   strcpy(test_calc_polish.wds[2].word, "+");
   strcpy(test_calc_polish.wds[3].word, ";");
   assert(!polish(&test_calc_polish, s));
   assert (fabs(test_calc_polish.result - 10.0) < 0.01);
   stack_free(s);
   s = stack_init();
   test_calc_polish = empty;
   strcpy(test_calc_polish.wds[0].word, "6");
   strcpy(test_calc_polish.wds[1].word, "4");
   strcpy(test_calc_polish.wds[2].word, "5");
   strcpy(test_calc_polish.wds[3].word, "+");
   strcpy(test_calc_polish.wds[4].word, "*");
   strcpy(test_calc_polish.wds[5].word, "25");
   strcpy(test_calc_polish.wds[6].word, "2");
   strcpy(test_calc_polish.wds[7].word, "3");
   strcpy(test_calc_polish.wds[8].word, "+");
   strcpy(test_calc_polish.wds[9].word, "/");
   strcpy(test_calc_polish.wds[10].word, "-");
   strcpy(test_calc_polish.wds[11].word, ";");
   assert(!polish(&test_calc_polish, s));
   assert (fabs(test_calc_polish.result - 49.0) < 0.01);
   stack_free(s);
   /* Test polish */
   s = stack_init();
   polish_test.cw = 0;
   polish_test.var[0].is_set = true;
   strcpy(polish_test.wds[0].word, "A");
   strcpy(polish_test.wds[1].word, "5");
   strcpy(polish_test.wds[2].word, "/");
   strcpy(polish_test.wds[3].word, ";");
   assert(!polish(&polish_test, s));
   strcpy(polish_test.wds[3].word, "FD");
   polish_test.cw = 0;
   assert(polish(&polish_test, s));
   strcpy(polish_test.wds[3].word, ";");
   strcpy(polish_test.wds[1].word, "XXX");
   polish_test.cw = 0;
   assert(polish(&polish_test, s));
   stack_free(s);


   /* Testing set of valid files */
   test_files = empty;
   load_file(&test_files, "test_files/valid_1.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_2.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_3.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_5.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_6.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_7.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_8.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_9.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_10.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_11.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_12.ttl");
   assert(!main_prog(&test_files));
   test_files = empty;
   load_file(&test_files, "test_files/valid_13.ttl");
   assert(!main_prog(&test_files));


   /* Testing set of invalid files
    checking line number and error code */
   test_files = empty;
   load_file(&test_files, "test_files/invalid_start.ttl");
   assert(main_prog(&test_files));
   assert(test_files.err == WRONG_START);

   test_files = empty;
   load_file(&test_files, "test_files/set_line6.ttl");
   assert(main_prog(&test_files));
   assert(test_files.err == WRONG_SET);

   test_files = empty;
   load_file(&test_files, "test_files/closing_bracket1.ttl");
   assert(main_prog(&test_files));
   assert(test_files.err == WRONG_BRACKET);

   test_files = empty;
   load_file(&test_files, "test_files/closing_bracket2.ttl");
   assert(main_prog(&test_files));
   assert(test_files.err == WRONG_BRACKET);


   test_files = empty;
   load_file(&test_files, "test_files/direc_line8.ttl");
   assert(main_prog(&test_files));
   assert(test_files.wds[test_files.cw].line_no == 8);
   assert(test_files.err == WRONG_DIRECTION);

   test_files = empty;
   load_file(&test_files, "test_files/direc_line26.ttl");
   assert(main_prog(&test_files));
   assert(test_files.wds[test_files.cw].line_no == 26);
   assert(test_files.err == WRONG_DIRECTION);

   test_files = empty;
   load_file(&test_files, "test_files/do_line2.ttl");
   assert(main_prog(&test_files));
   assert(test_files.wds[test_files.cw].line_no == 2);
   assert(test_files.err == WRONG_DO);

   test_files = empty;
   load_file(&test_files, "test_files/instr_line4.ttl");
   assert(main_prog(&test_files));
   assert(test_files.wds[test_files.cw].line_no == 4);
   assert(test_files.err == WRONG_INSTRUCTION);

   test_files = empty;
   load_file(&test_files, "test_files/instr_line13.ttl");
   assert(main_prog(&test_files));
   assert(test_files.wds[test_files.cw].line_no == 13);
   assert(test_files.err == WRONG_INSTRUCTION);

   test_files = empty;
   load_file(&test_files, "test_files/polish_line6.ttl");
   assert(main_prog(&test_files));
   assert(test_files.wds[test_files.cw].line_no == 6);
   assert(test_files.err == WRONG_POLISH);
   test_files = empty;

   load_file(&test_files, "test_files/polish_line7.ttl");
   assert(main_prog(&test_files));
   assert(test_files.wds[test_files.cw].line_no == 7);
   assert(test_files.err == WRONG_POLISH);

   test_files = empty;
   load_file(&test_files, "test_files/do_line4.ttl");
   assert(main_prog(&test_files));
   assert(test_files.wds[test_files.cw].line_no == 4);
   assert(test_files.err == WRONG_DO);

   test_files = empty;
   load_file(&test_files, "test_files/non_set_var_line4.ttl");
   assert(main_prog(&test_files));
   assert(test_files.wds[test_files.cw].line_no == 4);
   assert(test_files.err == VAR_NOT_SET);

   /* Test set */
   test_set.cw = 0;
   test_set.var[0].is_set = true;
   strcpy(test_set.wds[0].word, "SET");
   strcpy(test_set.wds[1].word, "A");
   strcpy(test_set.wds[2].word, ":=");
   strcpy(test_set.wds[3].word, "A");
   strcpy(test_set.wds[4].word, "5");
   strcpy(test_set.wds[5].word, "/");
   strcpy(test_set.wds[6].word, ";");
   assert(!set(&test_set));

   /* Testing do_loop */
   test_do_loop.cw = 0;
   strcpy(test_do_loop.wds[0].word, "DO");
   strcpy(test_do_loop.wds[1].word, "A");
   strcpy(test_do_loop.wds[2].word, "FROM");
   strcpy(test_do_loop.wds[3].word, "10");
   strcpy(test_do_loop.wds[4].word, "TO");
   strcpy(test_do_loop.wds[5].word, "9");
   strcpy(test_do_loop.wds[6].word, "{");
   strcpy(test_do_loop.wds[7].word, "}");
   assert(!do_loop(&test_do_loop));
   test_do_loop.cw = 0;
   strcpy(test_do_loop.wds[4].word, "XX");
   assert(do_loop(&test_do_loop));

   /* Testing var */
   test_var.cw = 0;
   strcpy(test_var.wds[0].word, "A");
   assert(!var(&test_var));
   strcpy(test_var.wds[0].word, "I");
   assert(!var(&test_var));
   strcpy(test_var.wds[0].word, "Z");
   assert(!var(&test_var));

   strcpy(test_var.wds[0].word, "a");
   assert(var(&test_var));
   strcpy(test_var.wds[0].word, "z");
   assert(var(&test_var));
   strcpy(test_var.wds[0].word, "AZ");
   assert(var(&test_var));
   strcpy(test_var.wds[0].word, "test");
   assert(var(&test_var));
   strcpy(test_var.wds[0].word, "");
   assert(var(&test_var));

   /* Testing op */
   test_op.cw = 0;
   strcpy(test_op.wds[0].word, "+");
   assert(!op(&test_op));
   strcpy(test_op.wds[0].word, "-");
   assert(!op(&test_op));
   strcpy(test_op.wds[0].word, "*");
   assert(!op(&test_op));
   strcpy(test_op.wds[0].word, "/");
   assert(!op(&test_op));

   strcpy(test_op.wds[0].word, "");
   assert(op(&test_op));
   strcpy(test_op.wds[0].word, "test");
   assert(op(&test_op));
   strcpy(test_op.wds[0].word, "++");
   assert(op(&test_op));
   strcpy(test_op.wds[0].word, "0");
   assert(op(&test_op));


   /* General testing */
   general.cw = 0;
   general.err = 0;
   strcpy(general.wds[0].word, "{");
   strcpy(general.wds[1].word, "FD");
   strcpy(general.wds[2].word, "3452");
   strcpy(general.wds[3].word, "LT");
   strcpy(general.wds[4].word, "660439");
   strcpy(general.wds[5].word, "RT");
   strcpy(general.wds[6].word, "1234");
   strcpy(general.wds[7].word, "}");
   assert(!main_prog(&general));
   assert(general.err == 0);

   general.cw = 0;
   strcpy(general.wds[3].word, "XX");
   assert(main_prog(&general));
   assert(general.err == WRONG_INSTRUCTION);
   general.cw = 0;
   general.err = 0;
   strcpy(general.wds[3].word, "LT");
   strcpy(general.wds[7].word, "XX");
   assert(main_prog(&general));
   assert(general.err == WRONG_INSTRUCTION);

   /* Testing main_prog */
   strcpy(test_main_prog.wds[0].word, "{");
   strcpy(test_main_prog.wds[1].word, "FD");
   strcpy(test_main_prog.wds[2].word, "10");
   strcpy(test_main_prog.wds[3].word, "}");
   test_main_prog.cw = 0;
   assert(!main_prog(&test_main_prog));
   assert(test_main_prog.err == 0);
   strcpy(test_main_prog.wds[0].word, "}");
   test_main_prog.cw = 0;
   assert(main_prog(&test_main_prog));
   assert(test_main_prog.err == WRONG_START);
   test_main_prog.err = 0;
   strcpy(test_main_prog.wds[0].word, "0");
   test_main_prog.cw = 0;
   assert(main_prog(&test_main_prog));
   assert(test_main_prog.err == WRONG_START);
   test_main_prog.err = 0;
   strcpy(test_main_prog.wds[0].word, "test");
   test_main_prog.cw = 0;
   assert(main_prog(&test_main_prog));
   assert(test_main_prog.err == WRONG_START);

   test1.cw = 0;
   strcpy(test1.wds[0].word, "{");
   strcpy(test1.wds[1].word, "FD");
   strcpy(test1.wds[2].word, "asd");
   strcpy(test1.wds[3].word, "}");
   main_prog(&test1);
   assert(test1.err == WRONG_DIRECTION);


   /* Testing varnum */
   test.cw = 0;
   strcpy(test.wds[0].word, "hello");
   assert(varnum(&test));
   strcpy(test.wds[0].word, "1");
   assert(!varnum(&test));
   strcpy(test.wds[0].word, "1234");
   assert(!varnum(&test));
   strcpy(test.wds[0].word, "0");
   assert(!varnum(&test));
   strcpy(test.wds[0].word, "12346345");
   assert(!varnum(&test));
   strcpy(test.wds[0].word, "10005");
   assert(!varnum(&test));
   test.var[0].is_set = true;
   strcpy(test.wds[0].word, "A");
   assert(!varnum(&test));
   test.var['L' - 'A'].is_set = true;
   strcpy(test.wds[0].word, "L");
   assert(!varnum(&test));
   test.var['Z' - 'A'].is_set = true;
   strcpy(test.wds[0].word, "Z");
   assert(!varnum(&test));
   strcpy(test.wds[0].word, "a");
   assert(varnum(&test));
   strcpy(test.wds[0].word, "AA");
   assert(varnum(&test));
   strcpy(test.wds[0].word, "5A");
   assert(varnum(&test));
   strcpy(test.wds[0].word, "A5");
   assert(varnum(&test));
   strcpy(test.wds[0].word, "1.5XX");
   assert(varnum(&test));
   strcpy(test.wds[0].word, "54XX63");
   assert(varnum(&test));

   /* Testing str_same */
   assert(str_same("t", "t"));
   assert(str_same("test", "test"));
   assert(str_same("1234", "1234"));
   assert(!str_same("t", "a"));
   assert(!str_same("test", "tesa"));
   assert(!str_same("test", "tes"));
   assert(!str_same("1234", "1235"));
   assert(!str_same("1234", "123"));

}
