/*
   KallistiGL 2.0.0

   quadmark.c
   (c) Ian micheal SDL+GLDC opengl
   (c)2018 Luke Benstead
   (c)2014 Josh Pearson
   (c)2002 Dan Potter, Paul Boese
*/

#include <kos.h>
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glkos.h"
#include "SDLDH/SDL.h"
#include "SDLDH/SDL_dreamcast.h"
/* screen width, height, and bit depth */
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     16
#include <stdlib.h>
#include <time.h>

enum { PHASE_HALVE, PHASE_INCR, PHASE_DECR, PHASE_FINAL };

int polycnt;
int phase = PHASE_HALVE;
float avgfps = -1;

void running_stats() {
    pvr_stats_t stats;
    pvr_get_stats(&stats);

    if(avgfps == -1)
        avgfps = stats.frame_rate;
    else
        avgfps = (avgfps + stats.frame_rate) / 2.0f;
}

void stats() {
    pvr_stats_t stats;

    pvr_get_stats(&stats);
    dbglog(DBG_DEBUG, "3D Stats: %d VBLs, frame rate ~%f fps\n",
           stats.vbl_count, stats.frame_rate);
}


int check_start() {
    maple_device_t *cont;
    cont_state_t *state;

    cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

    if(cont) {
        state = (cont_state_t *)maple_dev_status(cont);

        if(state)
            return state->buttons & CONT_START;
    }

    return 0;
}

/* This is our SDL surface */
SDL_Surface *surface;



/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
	glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0, 640, 0, 480, -100, 100);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void do_frame() {
    int x, y, z;
    int size;
    int i;
    float col;

    glBegin(GL_QUADS);

    for(i = 0; i < polycnt; i++) {
        x = rand() % 640;
        y = rand() % 480;
        z = rand() % 100 + 1;
        size = rand() % 50 + 1;
        col = (rand() % 255) * 0.00391f;

        glColor3f(col, col, col);
        glVertex3f(x - size, y - size, z);
        glVertex3f(x + size, y - size, z);
        glVertex3f(x + size, y + size, z);
        glVertex3f(x - size, y + size, z);
    }

    glEnd();

      /* Draw it to the screen */
    SDL_GL_SwapBuffers( );
}

time_t start1;
void switch_tests(int ppf) {
    printf("Beginning new test: %d polys per frame (%d per second at 60fps)\n",
           ppf * 2, ppf * 2 * 60);
    avgfps = -1;
    polycnt = ppf;
}

void check_switch() {
    time_t now;

    now = time(NULL);

    if(now >= (start1 + 5)) {
        start1 = time(NULL);
        printf("  Average Frame Rate: ~%f fps (%d pps)\n", avgfps, (int)(polycnt * avgfps * 2));

        switch(phase) {
            case PHASE_HALVE:

                if(avgfps < 55) {
                    switch_tests(polycnt / 1.2f);
                }
                else {
                    printf("  Entering PHASE_INCR\n");
                    phase = PHASE_INCR;
                }

                break;

            case PHASE_INCR:

                if(avgfps >= 55) {
                    switch_tests(polycnt + 15);
                }
                else {
                    printf("  Entering PHASE_DECR\n");
                    phase = PHASE_DECR;
                }

                break;

            case PHASE_DECR:

                if(avgfps < 55) {
                    switch_tests(polycnt - 30);
                }
                else {
                    printf("  Entering PHASE_FINAL\n");
                    phase = PHASE_FINAL;
                }

                break;

            case PHASE_FINAL:
                break;
        }
    }
}

int main(int argc, char **argv) {
    /* Initialize SDL for video output */
       SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
       SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
       SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 1);
       SDL_SetVideoMode(640,480,16,SDL_DOUBLEBUF | SDL_SWSURFACE | SDL_OPENGL);

  /* Set the title bar in environments that support it */
  SDL_WM_SetCaption("Jeff Molofee's GL Code Tutorial ... NeHe '99", NULL);
        SDL_ShowCursor(0);
  /* Loop, drawing and checking events */
  InitGL(640, 480);

    /* Start off with something obscene */
    switch_tests(200000 / 60);
    start1 = time(NULL);

    for(;;) {
        if(check_start())
            break;

        printf(" \r");
        do_frame();
        running_stats();
        check_switch();
    }

    stats();

    return 0;
}


