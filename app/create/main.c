#include "../../include/syscall.c"
#include "../../include/mem.c"
int main(int argc,char **argv)
{
	if(argc<3)
	{
		return 1;
	}
	if(!strcmp(argv[1],"-f"))
	{
		return mknod(argv[2],0100644,0);
	}
	if(!strcmp(argv[1],"-d"))
	{
		return mkdir(argv[2],0755);
	}
	if(argc<4)
	{
		return 1;
	}
	if(!strcmp(argv[1],"-l"))
	{
		return symlink(argv[3],argv[2]);
	}
	if(!strcmp(argv[1],"-L"))
	{
		return link(argv[3],argv[2]);
	}
	return 1;
}
