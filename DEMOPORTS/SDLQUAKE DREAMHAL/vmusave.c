#include "quakedef.h"
#include "errno.h"




int main (int argc, char **argv)
{


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