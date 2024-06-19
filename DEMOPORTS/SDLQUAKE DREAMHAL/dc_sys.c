/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// sys_null.h -- null system driver to aid porting efforts

#include "quakedef.h"
#include "errno.h"
#include "SDLDH/SDL.h"
#include "SDLDH/SDL_dreamcast.h" 
#include <kos.h> 
#include <arch/timer.h>
#include <arch/arch.h>
#include <dc/cdrom.h>




qboolean	isDedicated = false;

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES             10
FILE    *sys_handles[MAX_HANDLES];

int findhandle (void)
{
	int             i;
	
	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength (FILE *f)
{
	int             pos;
	int             end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead (char *path, int *hndl)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;
	
	return filelength(f);
}

int Sys_FileOpenWrite (char *path)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error ("Error opening %s", path);
	sys_handles[i] = f;
	
	return i;
}

void Sys_FileClose (int handle)
{
	fclose (sys_handles[handle]);
	sys_handles[handle] = NULL;
}

void Sys_FileSeek (int handle, int position)
{
	fseek (sys_handles[handle], position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
	return fread (dest, 1, count, sys_handles[handle]);
}

int Sys_FileWrite (int handle, void *data, int count)
{
	return fwrite (data, 1, count, sys_handles[handle]);
}

int     Sys_FileTime (char *path)
{
	FILE    *f;
	
	f = fopen(path, "rb");
	if (f)
	{
		fclose(f);
		return 1;
	}
	
	return -1;
}

void Sys_mkdir (char *path)
{
}



/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{
}


void Sys_Error (char *error, ...)
{
	va_list         argptr;
	char			strbuffer[2048];

	printf ("Sys_Error: ");   
	va_start (argptr, error);
	vsprintf (strbuffer, error, argptr);
	va_end (argptr);
	printf ("%s\n", strbuffer);

	exit (1);
}

void Sys_Printf (char *fmt, ...)
{
	va_list         argptr;
	char			strbuffer[2048];

	va_start (argptr, fmt);
	vsprintf (strbuffer, fmt, argptr);
	printf (strbuffer);
	va_end (argptr);
}

void Sys_Quit (void)
{
	
	exit(0);
}

double Sys_FloatTime (void)
{
	uint32 sec, msec;
	double time;
	timer_ms_gettime(&sec, &msec);
	time = sec + (msec / 1000.0);
	return time;
}



void Sys_Sleep (void)
{
}

void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}

//=============================================================================


void DC_CheckDrive()
{
	int disc_status, disc_type;

	// Get drive and disc status
	cdrom_get_status(&disc_status, &disc_type);

	// Drive is open - return to BIOS menu
	if(disc_status == CD_STATUS_OPEN)
		arch_menu();
}


char *quake_argvs[] =
{
	"quake"
};

void COM_AddGameDirectory (char *dir);

int main ()
{


//profiler_init("/pc/gmon.out");
//    profiler_start();






	int argc = sizeof(quake_argvs) / sizeof(char *);
	char **argv = quake_argvs;
	static quakeparms_t    parms;
	double time, oldtime, newtime;


	parms.memsize = 10*1024*1024;
	parms.membase = malloc (parms.memsize);


	parms.basedir = "/cd/quake";

	COM_InitArgv (argc, argv);
	parms.argc = com_argc;
	parms.argv = com_argv;

	
	printf ("Host_Init\n");
	Host_Init (&parms);


	oldtime = Sys_FloatTime() - 0.1;
	while(1)
	{
	
		newtime = Sys_FloatTime();
		time = newtime - oldtime;

        if (time > sys_ticrate.value*2)
            oldtime = newtime;
        else
            oldtime += time;

		Host_Frame(time);

	
		DC_CheckDrive();
	}

	return 0;
}
