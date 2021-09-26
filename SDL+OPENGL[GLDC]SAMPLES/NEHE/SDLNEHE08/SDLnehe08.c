/*  DREAMCAST
 *IAN MICHEAL Ported SDL+OPENGL USING SDL[DREAMHAL][GLDC][KOS2.0]2021
 * Cleaned and tested on dreamcast hardware by Ianmicheal
 *		This Code Was Created By Pet & Commented/Cleaned Up By Jeff Molofee
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit NeHe Productions At http://nehe.gamedev.net
 */

#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glkos.h"
#include "SDLDH/SDL.h"
#include "SDLDH/SDL_dreamcast.h"
#include <kos.h>
#define FPS 60
Uint32 waittime = 1000.0f/FPS;
Uint32 framestarttime = 0;
Sint32 delaytime;
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

/* lighting on/off (1 = on, 0 = off) */
int light = 0;

GLfloat xrot;   // x rotation 
GLfloat yrot;   // y rotation 
GLfloat xspeed; // x rotation speed
GLfloat yspeed; // y rotation speed

GLfloat z=-5.0f; // depth into the screen.

/* white ambient light at half intensity (rgba) */
GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };

/* super bright, full intensity diffuse light. */
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

/* position of light (x, y, z, (position of light)) */
GLfloat LightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };

GLuint	filter = 0;	/* Which Filter To Use (nearest/linear/mipmapped) */
GLuint	texture[3];	/* Storage for 3 textures. */
GLuint  blend = 0;      /* Turn blending on/off */

SDL_Surface *LoadBMP(char *filename)
{
    Uint8 *rowhi, *rowlo;
    Uint8 *tmpbuf, tmpch;
    SDL_Surface *image;
    int i, j;

    image = SDL_LoadBMP(filename);
    if ( image == NULL ) {
        fprintf(stderr, "Unable to load %s: %s\n", filename, SDL_GetError());
        return(NULL);
    }

    /* GL surfaces are upsidedown and RGB, not BGR :-) */
    tmpbuf = (Uint8 *)malloc(image->pitch);
    if ( tmpbuf == NULL ) {
        fprintf(stderr, "Out of memory\n");
        return(NULL);
    }
    rowhi = (Uint8 *)image->pixels;
    rowlo = rowhi + (image->h * image->pitch) - image->pitch;
    for ( i=0; i<image->h/2; ++i ) {
        for ( j=0; j<image->w; ++j ) {
            tmpch = rowhi[j*3];
            rowhi[j*3] = rowhi[j*3+2];
            rowhi[j*3+2] = tmpch;
            tmpch = rowlo[j*3];
            rowlo[j*3] = rowlo[j*3+2];
            rowlo[j*3+2] = tmpch;
        }
        memcpy(tmpbuf, rowhi, image->pitch);
        memcpy(rowhi, rowlo, image->pitch);
        memcpy(rowlo, tmpbuf, image->pitch);
        rowhi += image->pitch;
        rowlo -= image->pitch;
    }
    free(tmpbuf);
    return(image);
}

// Load Bitmaps And Convert To Textures
GLvoid LoadGLTextures(GLvoid)
{	
    // Load Texture
    SDL_Surface *image1;
    
    image1 = LoadBMP("/rd/glass.bmp");
    if (!image1) {
        SDL_Quit();
        exit(1);
    }

    // Create Textures	
    glGenTextures(3, &texture[0]);

    // texture 1 (poor quality scaling)
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // cheap scaling when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // cheap scaling when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->w, image1->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->pixels);

    // texture 2 (linear scaling)
    glBindTexture(GL_TEXTURE_2D, texture[1]);   // 2d texture (x and y size)
 //   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
 //   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->w, image1->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->pixels);

    // texture 3 (mipmapped scaling)
    glBindTexture(GL_TEXTURE_2D, texture[2]);   // 2d texture (x and y size)
 //   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
 //   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale linearly + mipmap when image smalled than texture
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->w, image1->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->pixels);

    // 2d texture, 3 colors, width, height, RGB in that order, byte data, and the data.
    glGenerateMipmapEXT(GL_TEXTURE_2D, 3, image1->w, image1->h, GL_RGB, GL_UNSIGNED_BYTE, image1->pixels); 
};

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
GLvoid InitGL(GLsizei Width, GLsizei Height)	// We call this right after our OpenGL window is created.
{
    glViewport(0, 0, Width, Height);
    LoadGLTextures();                           // load the textures.
    glEnable(GL_TEXTURE_2D);                    // Enable texture mapping.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
  //  glDepthFunc(GL_LESS);			// The Type Of Depth Test To Do
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix
    
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // set up light number 1.
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);  // add lighting. (ambient)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);  // add lighting. (diffuse).
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition); // set light position.
    glEnable(GL_LIGHT1);                             // turn light 1 on.
    
    /* setup blending */
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);			// Set The Blending Function For Translucency
    glColor4f(1.0f, 1.0f, 1.0f, 0.5);    





}

/* The main drawing function. */
void DrawGLScene()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, z);

    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);

    glBindTexture(GL_TEXTURE_2D, texture[filter]);

    glBegin(GL_QUADS);
    /* Front Face */
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    /* Back Face */
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    /* Top Face */
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f,  1.0f, -1.0f);
    /* Bottom Face */
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    /* Right face */
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f,  1.0f);
    /* Left Face */
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();

    xrot += xspeed;
    yrot += yspeed;
    // swap buffers to display, since we're double buffered.
    SDL_GL_SwapBuffers();
}

int main(int argc, char **argv) 
{  
    int done;
    Uint8 *keys;

        SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
        SDL_DC_ShowAskHz(SDL_FALSE);
    /* Map all START buttons as ESCAPE key */
        SDL_DC_MapKey(0, SDL_DC_START, SDLK_ESCAPE);
        SDL_DC_MapKey(0, SDL_DC_A, SDLK_l);
        SDL_DC_MapKey(0, SDL_DC_Y,SDLK_b);
        SDL_DC_MapKey(0, SDL_DC_X,SDLK_f);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 1);
         /* Create a 640x480 OpenGL screen */
    if ( SDL_SetVideoMode(640, 480, 0, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_OPENGL) == NULL ) {
        fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
        SDL_Quit();
        exit(2);
    }
         /* Enable joystick/pad 0 for keyboard and mouse emulation */
        SDL_JoystickEventState(SDL_ENABLE);
        SDL_JoystickOpen(0);
        SDL_ShowCursor(0);
    /* Set the title bar in environments that support it */
    SDL_WM_SetCaption("Jeff Molofee's GL Code Tutorial ... NeHe '99", NULL);

    /* Loop, drawing and checking events */
    InitGL(640, 480);
    done = 0;
    while ( ! done ) {
        DrawGLScene();

        /* This could go in a separate function */
        { SDL_Event event;
          while ( SDL_PollEvent(&event) ) {
              if ( event.type == SDL_QUIT ) {
                  done = 1;
              }
              if ( event.type == SDL_KEYDOWN ) {
                   switch(event.key.keysym.sym) {
                       case SDLK_ESCAPE:
                           done = 1;
                           break;
                       case SDLK_b:
	                   printf("Blend was: %d\n", blend);
	                   blend = blend ? 0 : 1;              // switch the current value of blend, between 0 and 1.
	                   printf("Blend is now: %d\n", blend);
	                   if (!blend) {
	                       glEnable(GL_BLEND);		    // Turn Blending On
	                       glDisable(GL_DEPTH_TEST);         // Turn Depth Testing Off
	                   } else {
	                       glDisable(GL_BLEND);              // Turn Blending Off
	                       glEnable(GL_DEPTH_TEST);          // Turn Depth Testing On
	                   }
	                   break;
                       case SDLK_l:
	                   printf("Light was: %d\n", light);
	                   light = light ? 0 : 1;              // switch the current value of light, between 0 and 1.
	                   printf("Light is now: %d\n", light);
	                   if (!light) {
	                       glDisable(GL_LIGHTING);
	                   } else {
	                       glEnable(GL_LIGHTING);
	                   }
	                   break;
                       case SDLK_f:
	                   printf("Filter was: %d\n", filter);
	                   filter += 1;
	                   if (filter>2)
                               filter = 0;
	                   printf("Filter is now: %d\n", filter);
	                   break;
                   }
               }
           }
         }

         /* Check current key state for special commands */
         keys = SDL_GetKeyState(NULL);
         if ( keys[SDLK_PAGEUP] == SDL_PRESSED ) {
             z-=0.02f;
         }
         if ( keys[SDLK_PAGEDOWN] == SDL_PRESSED ) {
             z+=0.02f;
         }
         if ( keys[SDLK_UP] == SDL_PRESSED ) {
             xspeed-=0.01f;
         }
         if ( keys[SDLK_DOWN] == SDL_PRESSED ) {
             xspeed+=0.01f;
         }
         if ( keys[SDLK_LEFT] == SDL_PRESSED ) {
             yspeed-=0.01f;
         }
         if ( keys[SDLK_RIGHT] == SDL_PRESSED ) {
             yspeed+=0.01f;
         }
    }
    SDL_Quit();
    return 1;
}
