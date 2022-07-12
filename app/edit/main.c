#include "../../include/malloc.c"
#include "../../include/mem.c"
#include "../../include/iformat.c"
#include "../../include/stat.c"
#include "../../include/ioctl/termios.c"
#include "../../include/signal.c"
#include "../../include/poll.c"
struct winsize winsz;
char *file_name;
int current_x;
#include "file.c"

struct termios term,old_term;
int mode; // 0 -- normal, 1 -- insert, 2 -- select, 3 -- command
struct file_pos select_pos;
char *clipboard;
unsigned long int clipboard_size;
char cmd_buf[64];
int cmd_size;

#include "cmd.c"
#include "undo.c"
int term_init(void)
{
	if(ioctl(0,TCGETS,&term))
	{
		return 1;
	}
	memcpy(&old_term,&term,sizeof(term));
	term.lflag&=~0xa;
	if(ioctl(0,TCSETS,&term))
	{
		return 1;
	}
	return 0;
}
int getc(void)
{
	struct pollfd pfd;
	int c;
	pfd.fd=0;
	pfd.events=POLLIN;
	poll(&pfd,1,-1);
	c=0;
	read(0,&c,1);
	return c;
}
int if_selected(struct file_pos *end,struct file_pos *pos)
{
	if(mode!=2)
	{
		return 0;
	}
	if(end->off<select_pos.off)
	{
		if(pos->off>=end->off&&pos->off<=select_pos.off)
		{
			return 1;
		}
	}
	else
	{
		if(pos->off<=end->off&&pos->off>=select_pos.off)
		{
			return 1;
		}
	}
	return 0;
}
void copy_selected_str(void)
{
	unsigned long int end_off;
	struct file_pos pos;
	int c;
	if(select_pos.off<current_pos.off)
	{
		end_off=current_pos.off;
		memcpy(&pos,&select_pos,sizeof(pos));
	}
	else
	{
		end_off=select_pos.off;
		memcpy(&pos,&current_pos,sizeof(pos));
	}
	free(clipboard);
	clipboard_size=0;
	clipboard=malloc(end_off-pos.off);
	if(clipboard==NULL)
	{
		return;
	}
	while(pos.off<=end_off)
	{
		c=file_getc(&pos);
		if(c==-1)
		{
			break;
		}
		clipboard[clipboard_size]=c;
		++clipboard_size;
		if(!file_pos_move_right(&pos))
		{
			break;
		}
	}
}
void del_selected_str(void)
{
	unsigned long int size,x;
	struct file_pos pos;
	int c1;
	if(select_pos.off<current_pos.off)
	{
		size=current_pos.off-select_pos.off+1;
		cursor_right();
		current_x_refine();
	}
	else
	{
		size=select_pos.off-current_pos.off+1;
		x=size;
		while(x)
		{
			cursor_right();
			current_x_refine();
			--x;
		}
	}
	while(size)
	{
		memcpy(&pos,&current_pos,sizeof(pos));
		if(file_pos_move_left(&pos))
		{
			c1=file_getc(&pos);
			op_push(c1|0x100,pos.off);
		}
		delc();
		current_x_refine();
		--size;
	}
}
void display_file(void)
{
	struct file_pos pos;
	int x,y;
	int cx,cy;
	int c,s,s1;
	int bufsize;
	char buf[512];
	y=0;
	cx=0;
	cy=0;
	s=0;
	bufsize=0;
	memcpy(&pos,view_pos,sizeof(pos));
	write(1,"\033[?25l\033[1;1H\x0f\033[0m",17);
	while(y<winsz.row-1)
	{
		x=current_x;
		s1=0;
		while(x)
		{
			c=file_getc(&pos);
			if(c==-1||c=='\n')
			{
				s1=1;
				break;
			}
			if(!file_pos_move_right(&pos))
			{
				s1=1;
				break;
			}
			--x;
		}
		if(!s1)
		{
			x=0;
			while(x<winsz.col)
			{
				c=file_getc(&pos);
				if(pos.off==current_pos.off)
				{
					cx=x;
					cy=y;
				}
				if(c=='\n')
				{
					break;
				}
				if(c>=32&&c<127)
				{
					if(if_selected(&current_pos,&pos))
					{
						if(bufsize>502)
						{
							write(1,buf,bufsize);
							bufsize=0;
						}
						memcpy(buf+bufsize,"\033[47m\033[30m",10);
						bufsize+=10;
					}
					if(bufsize==512)
					{
						write(1,buf,bufsize);
						bufsize=0;
					}
					buf[bufsize]=c;
					++bufsize;
					if(if_selected(&current_pos,&pos))
					{
						if(bufsize>508)
						{
							write(1,buf,bufsize);
							bufsize=0;
						}
						memcpy(buf+bufsize,"\033[0m",4);
						bufsize+=4;
					}
				}
				else if(c==-1)
				{
					break;
				}
				else if(c=='\t')
				{
					if(bufsize>502)
					{
						write(1,buf,bufsize);
						bufsize=0;
					}
					if(if_selected(&current_pos,&pos))
					{
						memcpy(buf+bufsize,"\033[47m \033[0m",10);
					}
					else
					{
						memcpy(buf+bufsize,"\033[42m \033[0m",10);
					}
					bufsize+=10;
				}
				else
				{
					if(bufsize>502)
					{
						write(1,buf,bufsize);
						bufsize=0;
					}
					if(if_selected(&current_pos,&pos))
					{
						memcpy(buf+bufsize,"\033[47m \033[0m",10);
					}
					else
					{
						memcpy(buf+bufsize,"\033[44m \033[0m",10);
					}
					bufsize+=10;
				}
				++x;
				if(!file_pos_move_right(&pos))
				{
					s=1;
					break;
				}
			}
		}
		if(!move_next_line(&pos))
		{
			s=1;
		}
		if(s)
		{
			if(current_pos_end)
			{
				cx=x;
				cy=y;
			}
		}
		while(x<winsz.col)
		{
			if(bufsize==512)
			{
				write(1,buf,bufsize);
				bufsize=0;
			}
			buf[bufsize]=' ';
			++bufsize;
			++x;
		}
		++y;
		if(s)
		{
			break;
		}
	}
	while(y<winsz.row-1)
	{
		x=0;
		while(x<winsz.col)
		{
			if(bufsize==512)
			{
				write(1,buf,bufsize);
				bufsize=0;
			}
			buf[bufsize]=' ';
			++bufsize;
			++x;
		}
		++y;
	}
	write(1,buf,bufsize);
	write(1,"                    \r",21);
	if(mode==1)
	{
		write(1,"Insert",6);
	}
	else if(mode==2)
	{
		write(1,"Select",6);
	}
	else if(mode==3)
	{
		write(1,">",1);
		x=0;
		while(x<cmd_size)
		{
			write(1,cmd_buf+x,1);
			++x;
		}
		while(x<64)
		{
			write(1," ",1);
			++x;
		}
	}
	strcpy(buf,"\033[");
	sprinti(buf,cy+1,1);
	strcat(buf,";");
	sprinti(buf,cx+1,1);
	strcat(buf,"H");
	write(1,buf,strlen(buf));
	write(1,"\033[?25h",6);
}
void handle_move(void)
{
	char c;
	if(getc()!='[')
	{
		return;
	}
	c=getc();
	if(c=='A')
	{
		cursor_up();
	}
	else if(c=='B')
	{
		cursor_down();
	}
	else if(c=='C')
	{
		cursor_right();
	}
	else if(c=='D')
	{
		cursor_left();
	}
	else if(c=='1')
	{
		if(getc()=='~')
		{
			if(mode==1||mode==2)
			{
				mode=0;
			}
		}
	}
	current_x_refine();
}
void keypress_handler(char c)
{
	if(c==27)
	{
		if(mode!=3)
		{
			handle_move();
		}
		return;
	}
	if(mode==0)
	{
		if(c=='I')
		{
			mode=1;
		}
		else if(c=='W')
		{
			save_file();
		}
		else if(c=='S')
		{
			mode=2;
			memcpy(&select_pos,&current_pos,sizeof(current_pos));
		}
		else if(c=='P')
		{
			int x;
			x=0;
			while(x<clipboard_size)
			{
				if(current_pos_end)
				{
					op_push(clipboard[x],current_pos.off+1);
					addc_end(clipboard[x]);
					current_pos_end=0;
					cursor_right();
					current_pos_end=1;
				}
				else
				{
					op_push(clipboard[x],current_pos.off);
					addc(clipboard[x]);
				}
				current_x_refine();
				++x;
			}
		}
		else if(c=='>')
		{
			mode=3;
			cmd_size=0;
		}
		else if(c=='U')
		{
			undo();
		}
		else if(c=='R')
		{
			redo();
		}
	}
	else if(mode==1)
	{
		if(c=='\n'||c=='\t'||c>=32&&c<127)
		{
			if(current_pos_end)
			{
				op_push(c,current_pos.off+1);
				addc_end(c);
				current_pos_end=0;
				cursor_right();
				current_pos_end=1;
			}
			else
			{
				op_push(c,current_pos.off);
				addc(c);
			}
			current_x_refine();
		}
		else if(c==127)
		{
			int c1;
			struct file_pos pos;
			if(current_pos_end)
			{
				current_pos_end=0;
				c1=file_getc(&current_pos);
				if(c1!=-1)
				{
					op_push(c1|0x100,current_pos.off);
				}
				if(!cursor_left())
				{
					current_pos.block=NULL;
					current_pos.pos=0;
					current_pos.off=0;
				}
				delc_end();
				current_pos_end=1;
			}
			else
			{
				memcpy(&pos,&current_pos,sizeof(pos));
				if(file_pos_move_left(&pos))
				{
					c1=file_getc(&pos);
					op_push(c1|0x100,pos.off);
				}
				delc();
			}
			current_x_refine();
		}
	}
	else if(mode==2)
	{
		if(c=='\n')
		{
			copy_selected_str();
			mode=0;
		}
		else if(c=='D')
		{
			copy_selected_str();
			del_selected_str();
			mode=0;
		}
	}
	else if(mode==3)
	{
		if(c>=32&&c<127)
		{
			if(cmd_size!=64)
			{
				cmd_buf[cmd_size]=c;
				++cmd_size;
			}
		}
		else if(c==127)
		{
			if(cmd_size)
			{
				--cmd_size;
			}
		}
		else if(c=='\n')
		{
			issue_cmd();
			mode=0;
		}
	}
}

int main(int argc,char **argv)
{
	struct stat st;
	int c;
	if(argc<2)
	{
		return 1;
	}
	if(ioctl(0,TIOCGWINSZ,&winsz))
	{
		return 3;
	}
	if(stat(argv[1],&st))
	{
		return 3;
	}
	if((st.mode&0170000)!=STAT_REG)
	{
		return 3;
	}
	file_name=argv[1];
	if(file_load())
	{
		return 3;
	}
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	if(term_init())
	{
		return 3;
	}
	write(1,"\033[2J",4);
	while(1)
	{
		display_file();
		c=getc();
		if(mode==0&&c=='Q')
		{
			break;
		}
		keypress_handler(c);
	}
	ioctl(0,TCSETS,&old_term);
	write(1,"\033[2J\033[1;1H",10);
	return 0;
}
