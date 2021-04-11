#include "extension.h"

int main(void)
{
   curser player = {0};
   FILE* fp = nfopen("Extension/extension.ttl", "w");
   fprintf(fp, "{\n");

   init_SDL(&player);
   run_SDL(&player, fp);
   fprintf(fp, "}");
   fclose(fp);
   SDL_Quit();
   atexit(SDL_Quit);
   return 0;
}

void run_SDL(curser* s, FILE* f)
{
   bool close_window = false;
   while (close_window == false)
   {
      Neill_SDL_UpdateScreen(&s->sw);
      while (SDL_PollEvent(&s->event))
      {
         switch (s->event.type)
         {
            /* If exit button is pressed close window */
            case SDL_QUIT:
               close_window = true;
               break;
            case SDL_KEYDOWN:
               switch (s->event.key.keysym.scancode)
               {
                  case SDL_SCANCODE_RETURN:
                  close_window = true;
                  break;

                  case SDL_SCANCODE_W:
                  case SDL_SCANCODE_UP:
                  update_coords(s, FD_SIZE);
                  if (s->file.rot) {
                     write_rot_to_file(s, f);
                  }
                  break;

                  case SDL_SCANCODE_A:
                  case SDL_SCANCODE_LEFT:
                  s->rot = mod_360(s->rot + ROT_SIZE);
                  s->file.rot -= ROT_SIZE;
                  if (s->file.fd) {
                     write_fd_to_file(s, f);
                  }
                  break;

                  case SDL_SCANCODE_D:
                  case SDL_SCANCODE_RIGHT:
                  s->rot = mod_360(s->rot + (DEG360 - ROT_SIZE));
                  s->file.rot += ROT_SIZE;
                  if (s->file.fd) {
                     write_fd_to_file(s, f);
                  }
                  break;

                  default:
                  break;
               }
               break;
            default:
               break;
         }
      }
   }
}

void init_SDL(curser* s)
{
   Neill_SDL_Init(&s->sw);
   Neill_SDL_SetDrawColour(&s->sw, RGBMAX, RGBMAX, RGBMAX);
   s->x = INIT_X_COORD;
   s->y = INIT_Y_COORD;
}

void write_rot_to_file(curser* s, FILE* f)
{
   if (s->file.rot < 0) {
      fprintf(f, "\tLT %d\n", s->file.rot * -1);
   }
   else {
      fprintf(f, "\tRT %d\n", s->file.rot);
   }
   s->file.rot = 0;
}

void write_fd_to_file(curser* s, FILE* f)
{
   fprintf(f, "\tFD %d\n", s->file.fd);
   s->file.fd = 0;
}

void update_coords(curser* s, float disp)
{
   int prev_x = s->x;
   int prev_y = s->y;
   s->file.fd += 50;
   s->x = s->x - (int)(disp * (sin(deg_to_rad(s->rot))));
   s->y = s->y - (int)(disp * (cos(deg_to_rad(s->rot))));
   SDL_RenderDrawLine(s->sw.renderer, prev_x, prev_y, s->x, s->y);
}

int mod_360(int num)
{
   if (num <= DEG360) {
      return num;
   }
   return num - DEG360;
}
