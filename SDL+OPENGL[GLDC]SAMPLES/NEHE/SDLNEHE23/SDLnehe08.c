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
#define BOOL    int
#define FALSE   0
#define TRUE    1

Uint8*	keys;
BOOL	active=TRUE;									// Window Active Flag Set To TRUE By Default
BOOL	fullscreen=FALSE;								// Fullscreen Flag Set To Fullscreen Mode By Default
BOOL	blend;											// Blending ON/OFF
BOOL	bp;												// Blend Button Pressed? (Modified, Just The Caption)
BOOL	fp;												// F1 Button Pressed?

const	float piover180 = 0.0174532925f;
float	heading = 0.0f;
float	xpos = 0.0f;
float	zpos = 0.0f;

GLfloat	yrot = 0.0f;											// Y Rotation
GLfloat	walkbias = 0;
GLfloat	walkbiasangle = 0;
GLfloat	lookupdown = 0.0f;
GLfloat	z=0.0f;											// Depth Into The Screen

GLuint	texture[5];										// Storage For 5 Textures (Modified... We'll Need This Later)

typedef struct tagVERTEX
{
	float x, y, z;
	float u, v;
} VERTEX;

typedef struct tagTRIANGLE
{
	int texture;										// Used For Texturing ( Add )
	VERTEX vertex[3];
} TRIANGLE;

typedef struct tagSECTOR
{
	int numtriangles;
	TRIANGLE* triangle;
} SECTOR;

SECTOR sector1;											// Our Model Goes Here:

int  adjust = 5;										// Speed Adjustment ( Add )

// Get Time In Milliseconds ( Add )
float TimerGetTime()
{
	return SDL_GetTicks();
}

void readstr(FILE *f,char *string)
{
	do
	{
		fgets(string, 255, f);
	} while ((string[0] == '/') || (string[0] == '\r') || (string[0] == '\n'));
	return;
}

void SetupWorld()
{
	float x, y, z, u, v;
	int loop, vert;
	int numtriangles;
	FILE *filein;
	char oneline[255];
	filein = fopen("/rd/world.txt", "rt");				// File To Load World Data From

	readstr(filein,oneline);
	sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

	sector1.triangle = (TRIANGLE *)malloc(numtriangles * sizeof(TRIANGLE));
	sector1.numtriangles = numtriangles;
	for (loop = 0; loop < numtriangles; loop++)
	{
		readstr(filein,oneline);						// ( Add )
		sscanf(oneline, "%i\n", &sector1.triangle[loop].texture);	// ( Add )

		for (vert = 0; vert < 3; vert++)
		{
			readstr(filein,oneline);
			sscanf(oneline, "%f %f %f %f %f %f %f", &x, &y, &z, &u, &v);
			sector1.triangle[loop].vertex[vert].x = x;
			sector1.triangle[loop].vertex[vert].y = y;
			sector1.triangle[loop].vertex[vert].z = z;
			sector1.triangle[loop].vertex[vert].u = u;
			sector1.triangle[loop].vertex[vert].v = v;
		}
	}
	fclose(filein);
	return;
}

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
    int Status=FALSE;									// Status Indicator
	int loop1;
    SDL_Surface *TextureImage[5];					// Create Storage Space For The Textures
    memset(TextureImage,0,sizeof(void *)*5);			// Set The Pointer To NULL
    if ((TextureImage[0]=LoadBMP("/rd/floor1.bmp")) &&	// Load The Floor Texture
        (TextureImage[1]=LoadBMP("/rd/light1.bmp")) &&	// Load the Light Texture
        (TextureImage[2]=LoadBMP("/rd/rustyblue.bmp")) &&	// Load the Wall Texture
        (TextureImage[3]=LoadBMP("/rd/crate.bmp")) &&	// Load the Crate Texture
        (TextureImage[4]=LoadBMP("/rd/weirdbrick.bmp")))	// Load the Ceiling Texture
	{   
		Status=TRUE;									// Set The Status To TRUE
		glGenTextures(5, &texture[0]);					// Create The Texture
		for (loop1=0; loop1<5; loop1++)				// Loop Through 5 Textures
		{
			glBindTexture(GL_TEXTURE_2D, texture[loop1]);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop1]->w, TextureImage[loop1]->h, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop1]->pixels);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		}
		for (loop1=0; loop1<5; loop1++)					// Loop Through 5 Textures
		{
			if (TextureImage[loop1])					// If Texture Exists
			{
				SDL_FreeSurface(TextureImage[loop1]);
			}
		}
	}
	return Status;										// Return The Status
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

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	if (!LoadGLTextures())								// Jump To Texture Loading Routine
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// Set The Blending Function For Translucency
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// This Will Clear The Background Color To Black
	glClearDepth(1.0);									// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);								// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glShadeModel(GL_SMOOTH);							// Enables Smooth Color Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	SetupWorld();

	return TRUE;										// Initialization Went OK
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	GLfloat x_m, y_m, z_m, u_m, v_m;
	GLfloat xtrans = -xpos;
	GLfloat ztrans = -zpos;
	GLfloat ytrans = -walkbias-0.25f;
	GLfloat sceneroty = 360.0f - yrot;

	int loop_m;
	int numtriangles;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The View

	glRotatef(lookupdown,1.0f,0,0);
	glRotatef(sceneroty,0,1.0f,0);
	
	glTranslatef(xtrans, ytrans, ztrans);
	numtriangles = sector1.numtriangles;
	
	// Process Each Triangle
	for (loop_m = 0; loop_m < numtriangles; loop_m++)
	{
		glBindTexture(GL_TEXTURE_2D, texture[sector1.triangle[loop_m].texture]);	// ( Modified )

		glBegin(GL_TRIANGLES);
			glNormal3f( 0.0f, 0.0f, 1.0f);
			x_m = sector1.triangle[loop_m].vertex[0].x;
			y_m = sector1.triangle[loop_m].vertex[0].y;
			z_m = sector1.triangle[loop_m].vertex[0].z;
			u_m = sector1.triangle[loop_m].vertex[0].u;
			v_m = sector1.triangle[loop_m].vertex[0].v;
			glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);
			
			x_m = sector1.triangle[loop_m].vertex[1].x;
			y_m = sector1.triangle[loop_m].vertex[1].y;
			z_m = sector1.triangle[loop_m].vertex[1].z;
			u_m = sector1.triangle[loop_m].vertex[1].u;
			v_m = sector1.triangle[loop_m].vertex[1].v;
			glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);
			
			x_m = sector1.triangle[loop_m].vertex[2].x;
			y_m = sector1.triangle[loop_m].vertex[2].y;
			z_m = sector1.triangle[loop_m].vertex[2].z;
			u_m = sector1.triangle[loop_m].vertex[2].u;
			v_m = sector1.triangle[loop_m].vertex[2].v;
			glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);
		glEnd();
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
	flags = SDL_OPENGL| SDL_DOUBLEBUF | SDL_SWSURFACE | SDL_OPENGL;
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
	float start;
	int x, y;

       SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
        SDL_DC_ShowAskHz(SDL_FALSE);
    /* Map all START buttons as ESCAPE key */
        SDL_DC_MapKey(0, SDL_DC_START, SDLK_ESCAPE);
        SDL_DC_MapKey(0, SDL_DC_A, SDLK_l);
        SDL_DC_MapKey(0, SDL_DC_Y,SDLK_b);
        SDL_DC_MapKey(0, SDL_DC_X,SDLK_f);
        SDL_DC_MapKey(0, SDL_DC_B,SDLK_PAGEUP);
       // SDL_DC_MapKey(0, SDL_DC_Y,SDLK_PAGEDOWN);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 1);
         /* Create a 640x480 OpenGL screen */
	// Grab and hide the mouse cursor so we always get relative motion
	// Note that this only works on Linux and Windows at the moment.
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_ShowCursor(0);
             /* Enable joystick/pad 0 for keyboard and mouse emulation */
        SDL_JoystickEventState(SDL_ENABLE);
        SDL_JoystickOpen(0);
        SDL_ShowCursor(0);
	// Create Our OpenGL Window
	if (!CreateGLWindow("Justin Eslinger's & NeHe's Advanced DirectInput Tutorial",640,480,16,fullscreen))	// ( Modified )
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

		start=TimerGetTime();				// Grab Timer Value Before We Draw	( Add )

		// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
		if ((active && !DrawGLScene()) || keys[SDLK_ESCAPE])	// Active?  Was There A Quit Received?
		{
			done=TRUE;							// ESC or DrawGLScene Signalled A Quit
		}
		else									// Not Time To Quit, Update Screen
		{
			while(TimerGetTime()<start+(float)(adjust*2.0f)) {} // Waste Cycles On Fast Systems ( Add )

			SDL_GL_SwapBuffers();					// Swap Buffers (Double Buffering)
		}


		SDL_GetRelativeMouseState(&x, &y);				// Get The Current Mouse Position	( Add )

		heading += (float)x/100 * 5;         // Update The Direction For Movement		( Add )
		yrot = heading;                                   // Update The Y Rotation	( Add )
		lookupdown -= (float)y/100 * 5;      // Update The X Rotation	( Add )

		if ( keys[SDLK_b] )			// B Key Being Pressed? ( Modified )
		{
			if (!bp)
			{
				bp = TRUE;					// Is The Blend Button Down?	( Add )
				blend=!blend;				// Toggle Blending On/Off
				if (!blend)					// Is Blending Off?
				{
					glDisable(GL_BLEND);	// If So, Disable Blending
					glEnable(GL_DEPTH_TEST);// Enable Depth Testing
				}
				else						// Otherwise
				{
					glEnable(GL_BLEND);		// Enable Blending
					glDisable(GL_DEPTH_TEST);	// Disable Depth Testing
				}
			}
		}
		else
		{
			bp = FALSE;						// Set 'bp' To False
		}

		if ( keys[SDLK_PAGEUP] )		// Page Up? ( Modified )
		{
			z-=0.10f;						// Decrease 'z' by 0.02f
		}

		if ( keys[SDLK_PAGEDOWN] )		// Page Down? ( Modified )
		{
			z+=0.10f;						// Increase 'z' by 0.02f
		}

		if ( keys[SDLK_UP] )		// Up Arrow ( Modified )
		{
			xpos -= (float)sin(heading*piover180) * 0.05f;
			zpos -= (float)cos(heading*piover180) * 0.05f;

			if (walkbiasangle >= 359.0f)
			{
				walkbiasangle = 0.0f;
			}
			else
			{
				walkbiasangle+= 10;
			}

			walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
		}

		if ( keys[SDLK_DOWN] )		// Down Arrow ( Modified )
		{
			xpos += (float)sin(heading*piover180) * 0.05f;
			zpos += (float)cos(heading*piover180) * 0.05f;

			if (walkbiasangle <= 1.0f)
			{
				walkbiasangle = 359.0f;
			}
			else
			{
				walkbiasangle-= 10;
			}

			walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
		}

		if ( keys[SDLK_LEFT] )		// ( Modified )
		{
			xpos += (float)sin((heading - 90)*piover180) * 0.05f;	// ( Modified )
			zpos += (float)cos((heading - 90)*piover180) * 0.05f;	// ( Modified )

			if (walkbiasangle <= 1.0f)		// ( Modified )
			{
				walkbiasangle = 359.0f;		// ( Modified )
			}
			else
			{
				walkbiasangle-= 10;			// ( Modified )
			}

			walkbias = (float)sin(walkbiasangle * piover180)/20.0f;	// ( Modified )
		}

		if ( keys[SDLK_RIGHT] )		// ( Modified )
		{
			xpos += (float)sin((heading + 90)*piover180) * 0.05f;	// ( Modified )
			zpos += (float)cos((heading + 90)*piover180) * 0.05f;	// ( Modified )

			if (walkbiasangle <= 1.0f)		// ( Modified )
			{
				walkbiasangle = 359.0f;		// ( Modified )
			}
			else
			{
				walkbiasangle-= 10;			// ( Modified )
			}

			walkbias = (float)sin(walkbiasangle * piover180)/20.0f;	// ( Modified )
		}

		if ( keys[SDLK_F1] )			// Is F1 Being Pressed?				( Modified )
		{
			if (!fp)						// If F1 Isn't Being "Held"			( Add )
			{
				fp = TRUE;					// Is The F1 Button Down?			( Add )
				KillGLWindow();				// Kill Our Current Window			( Modified )
				fullscreen=!fullscreen;		// Toggle Fullscreen/Windowed Mode	( Modified )

				// Recreate Our OpenGL Window
			    if (!CreateGLWindow("Justin Eslinger's & NeHe's Advanced Direct Input Tutorial",640,480,16,fullscreen))	// ( Modified )
				{
					done=TRUE;				// Quit If Window Was Not Created ( Modified )
				}
			}
		}
		else
		{
			fp = FALSE;						// Set 'fp' To False
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	SDL_Quit();
	return 0;										// Exit The Program
}
