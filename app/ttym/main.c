#include "../../include/syscall.c"
#include "../../include/mem.c"
#include "../../include/poll.c"
#include "../../include/iformat.c"
#include "../../include/ioctl/termios.c"
//ioctl 0x5606 -- switch to given tty
struct pollfd input_fds[64];
int num_input_fds;
int cfd;
int current_tty;

void input_init(void)
{
	int n;
	char name[32];
	int fd;
	n=0;
	while(n<64)
	{
		strcpy(name,"/dev/input/event");
		sprinti(name,n,1);
		fd=open(name,02000000,0);
		if(fd>=0)
		{
			input_fds[num_input_fds].fd=fd;
			input_fds[num_input_fds].events=POLLIN;
			++num_input_fds;
		}

		++n;
	}
}
void run_shell(char *tty)
{
	int fdi,fdo,fde;
	if(fork()==0)
	{
		while(1)
		{
			if(fork()==0)
			{
				char *argv[2];
				argv[0]="shell";
				argv[1]=NULL;
				fdi=open(tty,0,0);
				fdo=open(tty,2,0);
				fde=open(tty,2,0);
				if(fdi<0||fdo<0||fde<0)
				{
					exit(1);
				}
				dup2(fdi,0);
				dup2(fdo,1);
				dup2(fde,2);
				close(fdi);
				close(fdo);
				close(fde);
				setsid();
				setpgid(0,0);
				ioctl(0,TIOCSCTTY,1);
				execv("/bin/shell",argv);
				exit(1);
			}
			wait(NULL);
			sleep(0,300000);
		}
	}
}
struct input_event
{
	unsigned long int sec;
	unsigned long int usec;
	unsigned short int type;
	unsigned short int code;
	int value;
} event;
int main(void)
{
	int x;
	int alt_pressed;
	run_shell("/dev/tty1");
	run_shell("/dev/tty2");
	run_shell("/dev/tty3");
	run_shell("/dev/tty4");
	run_shell("/dev/tty5");
	do
	{
		input_init();
		sleep(0,300000);
	}
	while(num_input_fds==0);
	while(!valid(cfd=open("/dev/console",02000000,0)));
	alt_pressed=0;
	current_tty=1;
	ioctl(cfd,0x5606,1);
	while(1)
	{
		if(poll(input_fds,num_input_fds,-1)>0)
		{
			x=0;
			while(x<num_input_fds)
			{
				if(input_fds[x].revents)
				{
					input_fds[x].revents=0;
					if(read(input_fds[x].fd,&event,sizeof(event))==sizeof(event))
					{
						if(event.type==1)
						{
							if(event.code==56)
							{
								if(event.value==1)
								{
									alt_pressed=1;
								}
								else if(event.value==0)
								{
									alt_pressed=0;
								}
							}
							else if(event.value==1&&alt_pressed)
							{
								if(event.code==105)
								{
									--current_tty;
									if(current_tty<1)
									{
										current_tty=5;
									}
									ioctl(cfd,0x5606,current_tty);
								}
								else if(event.code==106)
								{
									++current_tty;
									if(current_tty>5)
									{
										current_tty=1;
									}
									ioctl(cfd,0x5606,current_tty);
								}
							}
						}
					}
				}
				++x;
			}		
		}
	}
}
