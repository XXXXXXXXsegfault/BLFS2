#include "../../include/malloc.c"
#include "../../include/mem.c"
#include "../../include/dirent.c"
#include "../../include/iformat.c"

struct proc
{
	struct proc *next;
	int pid;
	char name[36];
	long int mem;
} *proc,*proc_end;
void skip_entry(int fd)
{
	char c;
	while(read(fd,&c,1)==1)
	{
		if(c==32||c=='\n')
		{
			break;
		}
	}
}
long int read_entry(int fd)
{
	char c;
	long int ret;
	ret=0;
	while(read(fd,&c,1)==1)
	{
		if(c>='0'&&c<='9')
		{
			ret=ret*10+(c-'0');
		}
		else
		{
			break;
		}
	}
	return ret;
}
void proc_add(int pid)
{
	struct proc *node;
	int fd;
	char buf[48];
	int n;
	char c;
	node=malloc(sizeof(node));
	if(node==NULL)
	{
		return;
	}
	node->pid=pid;
	strcpy(buf,"/proc/");
	sprinti(buf,pid,1);
	strcat(buf,"/comm");
	fd=open(buf,0,0);
	n=0;
	if(fd>=0)
	{
		n=read(fd,node->name,16);
		close(fd);
	}
	node->name[n]=0;
	strcpy(buf,"/proc/");
	sprinti(buf,pid,1);
	strcat(buf,"/stat");
	fd=open(buf,0,0);
	node->mem=0;
	if(fd>=0)
	{
		skip_entry(fd);
		n+=5;
		while(n)
		{
			read(fd,&c,1);
			--n;
		}
		n=19;
		while(n)
		{
			skip_entry(fd);
			--n;
		}
		node->mem=read_entry(fd);
		close(fd);
	}
	node->next=NULL;
	if(proc_end)
	{
		proc_end->next=node;
	}
	else
	{
		proc=node;
	}
	proc_end=node;
}
void scan_proc(void)
{
	int fd;
	struct DIR db;
	struct dirent *dir;
	long int pid;
	fd=open("/proc",0,0);
	if(fd<0)
	{
		return;
	}
	dir_init(fd,&db);
	while(dir=readdir(&db))
	{
		if(dir->name[0]>='1'&&dir->name[0]<='9')
		{
			pid=0;
			sinputi(dir->name,&pid);
			proc_add(pid);
		}
	}
	close(fd);
}
int main(int argc,char **argv)
{
	struct proc *node;
	char buf[256];
	int l;
	scan_proc();
	node=proc;
	while(node)
	{
		if(node->mem)
		{
			l=strlen(node->name);
			node->name[l-1]=0;
			if(argc<2||!strcmp(argv[1],node->name))
			{
				buf[0]=0;
				sprinti(buf,node->pid,1);
				l=strlen(buf);
				while(l<14)
				{
					buf[l]=32;
					++l;
				}
				strcpy(buf+14,node->name);
				l=strlen(buf);
				while(l<36)
				{
					buf[l]=32;
					++l;
				}
				buf[l]=0;
				sprinti(buf,node->mem,1);
				strcat(buf,"\n");
				write(1,buf,strlen(buf));
			}
		}
		node=node->next;
	}
	return 0;
}
