#include <ctype.h>
#include "quakedef.h"
#include "d_local.h"
#include "alloca.h"
#include <stdio.h> 
#include <stdlib.h> 
#include "SDLDH/SDL.h"
#include "SDLDH/SDL_dreamcast.h" 
#include "SDLDH/SDL_timer.h"
#include "errno.h"
#include <signal.h>
#include <arch/timer.h>
#include <arch/arch.h>
#include <dc/cdrom.h>

qboolean	isDedicated = false;

int noconinput = 0;

char *basedir = "/cd/quake";
char *cachedir = 1024;

cvar_t  sys_linerefresh = {"sys_linerefresh","0"};// set for entity display
cvar_t  sys_nostdout = {"sys_nostdout","0"};

// =======================================================================
// General routines
// =======================================================================

void Sys_DebugNumber(int y, int val)
{
}

void Sys_Printf (char *fmt, ...)
{
	va_list		argptr;
	char		text[1024];
	
	va_start (argptr,fmt);
	vsprintf (text,fmt,argptr);
	va_end (argptr);
	fprintf(stderr, "%s", text);
	
	//Con_Print (text);
}




void Sys_Init(void)
{
}


void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}

void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{
}


void Sys_Error (char *error, ...)
{ 
    va_list     argptr;
    char        string[1024];

    va_start (argptr,error);
    vsprintf (string,error,argptr);
    va_end (argptr);
	fprintf(stderr, "Error: %s\n", string);

	Host_Shutdown ();
	exit (1);

} 

void Sys_Warn (char *warning, ...)
{ 
    va_list     argptr;
    char        string[1024];
    
    va_start (argptr,warning);
    vsprintf (string,warning,argptr);
    va_end (argptr);
	fprintf(stderr, "Warning: %s", string);
} 

// from dc_system


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

// from dc_system

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

 
// from dc_system


void Sys_DebugLog(char *file, char *fmt, ...)
{
    va_list argptr; 
    static char data[1024];
    FILE *fp;
    
    va_start(argptr, fmt);
    vsprintf(data, fmt, argptr);
    va_end(argptr);
    fp = fopen(file, "a");
    fwrite(data, strlen(data), 1, fp);
    fclose(fp);
}

void Sys_Quit (void)
{

	exit(0);
}



double Sys_FloatTime (void)
{


    struct timeval tp;
    struct timezone tzp; 
    static int      secbase; 
    
    gettimeofday(&tp, &tzp);  

    if (!secbase)
    {
        secbase = tp.tv_sec;
        return tp.tv_usec/1000000.0;
    }

    return (tp.tv_sec - secbase) + tp.tv_usec/1000000.0;


}

// =======================================================================
// Sleeps for microseconds
// =======================================================================

static volatile int oktogo;

void alarm_handler(int x)
{
	oktogo=1;
}

byte *Sys_ZoneBase (int *size)
{

	char *QUAKEOPT = getenv("QUAKEOPT");

	*size = 0xc00000;
	if (QUAKEOPT)
	{
		while (*QUAKEOPT)
			if (tolower(*QUAKEOPT++) == 'm')
			{
				*size = atof(QUAKEOPT) * 1024*1024;
				break;
			}
	}
	return malloc (*size);

}

void Sys_LineRefresh(void)
{
}

void Sys_Sleep(void)
{
	SDL_Delay(1);
}

void floating_point_exception_handler(int whatever)
{
//	Sys_Warn("floating point exception\n");
	signal(SIGFPE, floating_point_exception_handler);
}

void moncontrol(int x)
{
}

char *get_savedir(void)
{
	static char savedir[]="/vmu/a1";

	int port,unit;
	uint8 addr;
	addr = maple_first_vmu();
	if (addr==0) return "dmy";
	maple_raddr(addr,&port,&unit);
	savedir[5] = 'a' + port;
	savedir[6] = '0' + unit;

	return savedir;
}




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
	    cdrom_init();
    /* Do an initial initialization */
    cdrom_reinit();
	int argc = sizeof(quake_argvs) / sizeof(char *);
	char **argv = quake_argvs;
	static quakeparms_t    parms;
	double time, oldtime, newtime;


	parms.memsize = 8*1024*1024;
	parms.membase = malloc (parms.memsize);
    parms.cachedir = cachedir;

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
	
// graphic debugging aids
        if (sys_linerefresh.value)
            Sys_LineRefresh ();
    }

}



