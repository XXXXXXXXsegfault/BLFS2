#include "../../include/malloc.c"
#include "../../include/signal.c"
#include "../../include/poll.c"
#include "../../include/iformat.c"
#include "../../include/ioctl/termios.c"
struct termios term,oldterm;
struct winsize winsz;

void term_init(void)
{
	oldterm.iflag=0x6506;
	oldterm.oflag=0x5;
	oldterm.cflag=0x4bf;
	oldterm.lflag=0x8a3b;
	memcpy(oldterm.cc,"\x03\x1c\x7f\x15\x04\x00\x01\x00\x11\x13\x1a\x00\x12\x0f\x17\x16",16);
	oldterm.ispeed=0xf;
	oldterm.ospeed=0xf;
	memcpy(&term,&oldterm,sizeof(term));
	term.lflag&=~0xa;
	if(ioctl(0,TCSETS,&term))
	{
		exit(1);
	}
	ioctl(0,TIOCGWINSZ,&winsz);
}
#include "read_line.c"
void release_argv(char **argv,int argc)
{
	if(argc==-1)
	{
		argc=0;
		while(argv[argc])
		{
			free(argv[argc]);
			++argc;
		}
	}
	else
	{
		while(argc)
		{
			--argc;
			free(argv[argc]);
		}
	}
	free(argv);
}
char **parse_cmdline(char *line)
{
	char c;
	char **argv,**argv_new,*str;
	int count_args;
	count_args=0;
	str=NULL;
	argv=malloc(sizeof(void *)*16);
	if(argv==NULL)
	{
		return NULL;
	}
	while(c=*line)
	{
		if(c==32)
		{
			if(str)
			{
				argv[count_args]=str;
				++count_args;
				if((count_args&0xf)==0xf)
				{
					argv_new=malloc(sizeof(void *)*(count_args+17));
					if(argv_new==NULL)
					{
						release_argv(argv,count_args);
						return NULL;
					}
					memcpy(argv_new,argv,count_args*sizeof(void *));
					free(argv);
					argv=argv_new;
				}
			}
			str=NULL;
		}
		else 
		{
			if(c=='\\')
			{
				++line;
				c=*line;
				if(c==0)
				{
					break;
				}
			}
			str=str_app(str,c);
			if(str==NULL)
			{
				release_argv(argv,count_args);
				return NULL;
			}
		}
		++line;
	}
	if(str)
	{
		argv[count_args]=str;
		++count_args;
		if((count_args&0xf)==0xf)
		{
			argv_new=malloc(sizeof(void *)*(count_args+17));
			if(argv_new==NULL)
			{
				release_argv(argv,count_args);
				return NULL;
			}
			memcpy(argv_new,argv,count_args*sizeof(void *));
			free(argv);
			argv=argv_new;
		}
	}
	if(count_args==0)
	{
		free(argv);
		return NULL;
	}
	argv[count_args]=NULL;
	return argv;
}
#include "internal_cmd.c"
int is_path(char *str)
{
	while(*str)
	{
		if(*str=='/')
		{
			return 1;
		}
		++str;
	}
	return 0;
}
int main(void)
{
	char *line,**argv;
	char *program_path;
	unsigned int status;
	char msg[256];
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	term_init();
	while(1)
	{
		write(1,"[BLFS2] ",8);
		line=read_line();
		if(line)
		{
			argv=parse_cmdline(line);
			free(line);
		}
		else
		{
			argv=NULL;
		}
		if(argv)
		{
			if(exec_internal_cmd(argv))
			{
				program_path=malloc(strlen(argv[0])+10);
				if(program_path)
				{
					if(is_path(argv[0]))
					{
						strcpy(program_path,argv[0]);
					}
					else
					{
						strcpy(program_path,"/bin/");
						strcat(program_path,argv[0]);
					}
				}
				ioctl(0,TCSETS,&oldterm);
				if(fork()==0)
				{
					signal(SIGINT,SIG_DFL);
					signal(SIGQUIT,SIG_DFL);
					status=execv(program_path,argv);
					exit(status);
				}
				free(program_path);	
				release_argv(argv,-1);
				wait(&status);
				ioctl(0,TCSETS,&term);
				if(status&0x7f)
				{
					strcpy(msg,"Program terminated with signal ");
					sprinti(msg,status&0x7f,1);
				}
				else
				{
					strcpy(msg,"Program exited with code ");
					sprinti(msg,status>>8&0xff,1);
				}
				write(1,msg,strlen(msg));
				write(1,"\n",1);
			}
		}
	}
}
