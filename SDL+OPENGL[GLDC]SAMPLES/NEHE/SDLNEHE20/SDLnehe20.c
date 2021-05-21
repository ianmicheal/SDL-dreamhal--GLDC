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
#include "SDL.h"
#include <kos.h>
#include <SDL_dreamcast.h>

#define FPS 60
Uint32 waittime = 1000.0f/FPS;
Uint32 framestarttime = 0;
Sint32 delaytime;
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);
/*
 *		This Code Was Created By Jeff Molofee 2000
 *		And Modified By Giuseppe D'Agata (waveform@tiscalinet.it)
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */

#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>	// Header File For The OpenGL32 Library
#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#endif
#include "SDL.h"

#define BOOL    int
#define FALSE   0
#define TRUE    1


Uint8*	keys;			// Array Used For The Keyboard Routine
BOOL	active=TRUE;		// Window Active Flag Set To TRUE By Default
BOOL	fullscreen=FALSE;	// Fullscreen Flag Set To Fullscreen Mode By Default
BOOL	masking=TRUE;		// Masking On/Off
BOOL	mp;					// M Pressed?
BOOL	sp;					// Space Pressed?
BOOL	scene;				// Which Scene To Draw

GLuint	texture[5];			// Storage For Our Five Textures
GLuint	loop;				// Generic Loop Variable

GLfloat	roll;				// Rolling Texture


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

int LoadGLTextures()                                    // Load Bitmaps And Convert To Textures
{
        int Status=FALSE;                               // Status Indicator
        SDL_Surface *TextureImage[5];               // Create Storage Space For The Textures
        memset(TextureImage,0,sizeof(void *)*5);        // Set The Pointer To NULL

        if ((TextureImage[0]=LoadBMP("/rd/logo.bmp")) &&	// Logo Texture
			(TextureImage[1]=LoadBMP("/rd/mask1.bmp")) &&	// First Mask
			(TextureImage[2]=LoadBMP("/rd/image1.bmp")) &&	// First Image
			(TextureImage[3]=LoadBMP("/rd/mask2.bmp")) &&	// Second Mask
			(TextureImage[4]=LoadBMP("/rd/image2.bmp")))	// Second Image
        {
                Status=TRUE;                            // Set The Status To TRUE
                glGenTextures(5, &texture[0]);          // Create Five Textures

				for (loop=0; loop<5; loop++)			// Loop Through All 5 Textures
				{
	                glBindTexture(GL_TEXTURE_2D, texture[loop]);
			        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->w, TextureImage[loop]->h, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->pixels);
				}
        }
		for (loop=0; loop<5; loop++)						// Loop Through All 5 Textures
		{
	        if (TextureImage[loop])							// If Texture Exists
		    {
				SDL_FreeSurface(TextureImage[loop]);
			}
		}
        return Status;                                  // Return The Status
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);	// Calculate Window Aspect Ratio
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	if (!LoadGLTextures())								// Jump To Texture Loading Routine
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Clear The Background Color To Black
	glClearDepth(1.0);									// Enables Clearing Of The Depth Buffer
	glDisable(GL_BLEND);								// Disable Blending
	glEnable(GL_DEPTH_TEST);							// Enable Depth Testing
	glShadeModel(GL_SMOOTH);							// Enables Smooth Color Shading
	glEnable(GL_TEXTURE_2D);							// Enable 2D Texture Mapping
	return TRUE;										// Initialization Went OK
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glEnable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The Modelview Matrix
	glTranslatef(0.0f,0.0f,-2.0f);						// Move Into The Screen 5 Units
    glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[0]);			// Select Our Logo Texture
	glBegin(GL_QUADS);									// Start Drawing A Textured Quad
		glTexCoord2f(0.0f, -roll+0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);	// Bottom Left
		glTexCoord2f(3.0f, -roll+0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);	// Bottom Right
		glTexCoord2f(3.0f, -roll+3.0f); glVertex3f( 1.1f,  1.1f,  0.0f);	// Top Right
		glTexCoord2f(0.0f, -roll+3.0f); glVertex3f(-1.1f,  1.1f,  0.0f);	// Top Left
	glEnd();											// Done Drawing The Quad
	glPopMatrix();
	glPushMatrix();
//	glEnable(GL_BLEND);									// Enable Blending
	glDisable(GL_DEPTH_TEST);							// Disable Depth Testing

	if (masking)										// Is Masking Enabled?
	{
		glBlendFunc(GL_DST_COLOR,GL_ZERO);				// Blend Screen Color With Zero (Black)
	}

	if (scene)											// Are We Drawing The Second Scene?
	{
		glTranslatef(0.0f,0.0f,-1.0f);					// Translate Into The Screen One Unit
		glRotatef(roll*360,0.0f,0.0f,1.0f);				// Rotate On The Z Axis 360 Degrees.
		if (masking)									// Is Masking On?
		{
			glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, texture[3]);	// Select The Second Mask Texture
			glBegin(GL_QUADS);							// Start Drawing A Textured Quad
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);	// Bottom Left
				glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);	// Bottom Right
				glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.1f,  1.1f,  0.0f);	// Top Right
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.1f,  1.1f,  0.0f);	// Top Left
			glEnd();									// Done Drawing The Quad
			glPopMatrix();
	        glPushMatrix();
		}
        glPushMatrix();
		glBlendFunc(GL_ONE, GL_ONE);					// Copy Image 2 Color To The Screen
		glBindTexture(GL_TEXTURE_2D, texture[4]);		// Select The Second Image Texture
		glBegin(GL_QUADS);								// Start Drawing A Textured Quad
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);	// Bottom Left
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);	// Bottom Right
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.1f,  1.1f,  0.0f);	// Top Right
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.1f,  1.1f,  0.0f);	// Top Left
		glEnd();	
		glPopMatrix();
	    glPushMatrix();									// Done Drawing The Quad
	}
	else												// Otherwise
	{
		if (masking)									// Is Masking On?
		{
			glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, texture[1]);	// Select The First Mask Texture
			glBegin(GL_QUADS);							// Start Drawing A Textured Quad
				glTexCoord2f(roll+0.0f, 0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);	// Bottom Left
				glTexCoord2f(roll+4.0f, 0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);	// Bottom Right
				glTexCoord2f(roll+4.0f, 4.0f); glVertex3f( 1.1f,  1.1f,  0.0f);	// Top Right
				glTexCoord2f(roll+0.0f, 4.0f); glVertex3f(-1.1f,  1.1f,  0.0f);	// Top Left
			glEnd();
			glPopMatrix();
	        glPushMatrix();									// Done Drawing The Quad
		}
        glPushMatrix();
		glBlendFunc(GL_ONE, GL_ONE);					// Copy Image 1 Color To The Screen
		glBindTexture(GL_TEXTURE_2D, texture[2]);		// Select The First Image Texture
		glBegin(GL_QUADS);								// Start Drawing A Textured Quad
			glTexCoord2f(roll+0.0f, 0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);	// Bottom Left
			glTexCoord2f(roll+4.0f, 0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);	// Bottom Right
			glTexCoord2f(roll+4.0f, 4.0f); glVertex3f( 1.1f,  1.1f,  0.0f);	// Top Right
			glTexCoord2f(roll+0.0f, 4.0f); glVertex3f(-1.1f,  1.1f,  0.0f);	// Top Left
		glEnd();										// Done Drawing The Quad
	    glPopMatrix();
	    glPushMatrix();
	}

	glEnable(GL_DEPTH_TEST);							// Enable Depth Testing
	glDisable(GL_BLEND);								// Disable Blending

	roll+=0.002f;										// Increase Our Texture Roll Variable
	if (roll>1.0f)										// Is Roll Greater Than One
	{
		roll-=1.0f;										// Subtract 1 From Roll
	}

	return TRUE;										// Everything Went OK
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, BOOL fullscreenflag)
{
	Uint32 flags;

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag
	flags = SDL_OPENGL;
	if ( fullscreenflag ) {
		flags |= SDL_FULLSCREEN;
	}
	if ( SDL_SetVideoMode(width, height, 0, flags) == NULL ) {
		return FALSE;
	}
	SDL_WM_SetCaption(title, "opengl");

	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}



main(int argc, char *argv[])
{
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

 
        SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
        SDL_DC_ShowAskHz(SDL_FALSE);

    /* Map all START buttons as ESCAPE key */
        SDL_DC_MapKey(0, SDL_DC_START, SDLK_ESCAPE);
        SDL_DC_MapKey(0, SDL_DC_A, SDLK_m);
        SDL_DC_MapKey(0, SDL_DC_Y,SDLK_b);
        SDL_DC_MapKey(0, SDL_DC_X,SDLK_SPACE);
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

	// Create Our OpenGL Window
	if (!CreateGLWindow("NeHe's Masking Tutorial",640,480,16,fullscreen))
	{
		SDL_Quit();
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_QUIT:
					done=TRUE;							// If So done=TRUE
					break;
				default:
					break;
			}
		}
		keys = SDL_GetKeyState(NULL);

		// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
		if ((active && !DrawGLScene()) || keys[SDLK_ESCAPE])	// Active?  Was There A Quit Received?
		{
			done=TRUE;							// ESC or DrawGLScene Signalled A Quit
		}
		else									// Not Time To Quit, Update Screen
		{
			SDL_GL_SwapBuffers();					// Swap Buffers (Double Buffering)
		}

		if (keys[SDLK_SPACE] && !sp)				// Is Space Being Pressed?
		{
			sp=TRUE;						// Tell Program Spacebar Is Being Held
			scene=!scene;					// Toggle From One Scene To The Other
		}
		if (!keys[SDLK_SPACE])						// Has Spacebar Been Released?
		{
			sp=FALSE;						// Tell Program Spacebar Has Been Released
		}

		if (keys[SDLK_m] && !mp)				// Is M Being Pressed?
		{
			mp=TRUE;						// Tell Program M Is Being Held
			masking=!masking;				// Toggle Masking Mode OFF/ON
		}
		if (!keys[SDLK_m])						// Has M Been Released?
		{
			mp=FALSE;						// Tell Program That M Has Been Released
		}

		if (keys[SDLK_F1])						// Is F1 Being Pressed?
		{
			KillGLWindow();						// Kill Our Current Window
			fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
			// Recreate Our OpenGL Window
			if (!CreateGLWindow("NeHe's Masking Tutorial",640,480,16,fullscreen))
			{
				done=TRUE;						// Quit If Window Was Not Created
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	SDL_Quit();
	return 0;
}

