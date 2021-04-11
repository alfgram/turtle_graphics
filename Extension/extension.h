#include "../Neill_files/neillsdl2.h"
#include "../Neill_files/general.h"

#define RGBMAX 255
#define DEG360 360.0
#define DEG180 180.0
#define FD_SIZE 50
#define ROT_SIZE 10
#define INIT_X_COORD 400
#define INIT_Y_COORD 300
/* Converts N in degrees to radians */
#define deg_to_rad(N) (N * M_PI / DEG180)

struct file_write
{
   int fd;
   int rot;
};
typedef struct file_write file_write;

struct curser
{
   int x;
   int y;
   int rot;
   file_write file;

   SDL_Simplewin sw;
   SDL_Event event;
};
typedef struct curser curser;

void update_coords(curser* s, float disp);
int mod_360(int num);
void write_rot_to_file(curser* s, FILE* f);
void write_fd_to_file(curser* s, FILE* f);
void init_SDL(curser* s);
void run_SDL(curser* s, FILE* f);
