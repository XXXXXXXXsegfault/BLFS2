#include "../../include/malloc.c"
#include "../../include/dirent.c"
#include "../../include/mem.c"

struct filename
{
	struct filename *next;
	char name[256];
} *filename_head;
int main(int argc,char **argv)
{
	char *lspath;
	int fd,err;
	struct DIR db;
	struct dirent *dir;
	struct filename *node,*p,*pp;
	int x;
	if(argc<2)
	{
		lspath=".";
	}
	else
	{
		lspath=argv[1];
	}
	fd=open(lspath,0,0);
	if(fd<0)
	{
		return fd;
	}
	dir_init(fd,&db);
	while(dir=readdir(&db))
	{
		node=malloc(sizeof(*node));
		if(node==NULL)
		{
			return 2;
		}
		strcpy(node->name,dir->name);
		p=filename_head;
		pp=NULL;
		while(p&&strcmp(node->name,p->name)>0)
		{
			pp=p;
			p=p->next;
		}
		node->next=p;
		if(pp==NULL)
		{
			filename_head=node;
		}
		else
		{
			pp->next=node;
		}
	}
	p=filename_head;
	while(p)
	{
		x=0;
		while(p->name[x])
		{
			if(p->name[x]==' '||p->name[x]=='\\')
			{
				write(1,"\\",1);
			}
			write(1,p->name+x,1);
			++x;
		}
		write(1,"   ",3);
		p=p->next;
	}
	close(fd);
	write(1,"\n",1);
	return 0;
}
