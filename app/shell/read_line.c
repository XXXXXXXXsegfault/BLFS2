
char *str_app(char *str,char c)
{
	char *ret;
	int l;
	if(str==NULL)
	{
		ret=malloc(2);
		if(ret==NULL)
		{
			return NULL;
		}
		ret[0]=c;
		ret[1]=0;
		return ret;
	}
	else
	{
		l=strlen(str);
		ret=malloc(l+2);
		if(ret==NULL)
		{
			free(str);
			return NULL;
		}
		memcpy(ret,str,l);
		ret[l]=c;
		ret[l+1]=0;
		free(str);
		return ret;
	}
}
char *str_ins(char *str,int x,char c)
{
	char *ret;
	int i;
	char c1;
	ret=NULL;
	i=0;
	if(str==NULL)
	{
		if(x==0)
		{
			ret=str_app(ret,c);
		}
		return ret;
	}
	while(c1=str[i])
	{
		if(i==x)
		{
			ret=str_app(ret,c);
			if(ret==NULL)
			{
				return NULL;
			}
		}
		ret=str_app(ret,c1);
		++i;
	}
	if(i==x)
	{
		ret=str_app(ret,c);
		if(ret==NULL)
		{
			return NULL;
		}
	}
	free(str);
	return ret;
}
char *str_del(char *str,int x)
{
	char *ret;
	int i;
	char c1;
	ret=NULL;
	i=0;
	while(c1=str[i])
	{
		if(i!=x)
		{
			ret=str_app(ret,c1);
			if(ret==NULL)
			{
				return NULL;
			}
		}
		++i;
	}
	free(str);
	return ret;
	
}
int getc(void)
{
	struct pollfd pfd;
	int c;
	pfd.fd=0;
	pfd.events=POLLIN;
	pfd.revents=0;
	poll(&pfd,1,-1);
	c=0;
	read(0,&c,1);
	return c;
}
char *read_line(void)
{
	int current_x;
	char *line;
	int c,l;
	char buf[32];
	current_x=0;
	line=NULL;
	while((c=getc())!='\n')
	{
		write(1,"\033[?25l",6);
		if(current_x+8>=winsz.col)
		{
			strcpy(buf,"\033[");
			sprinti(buf,(current_x+8)/winsz.col,1);
			strcat(buf,"A");
			write(1,buf,strlen(buf));
		}
		if(c>=32&&c<=126)
		{
			line=str_ins(line,current_x,c);
			++current_x;
		}
		else if(c==127)
		{
			if(current_x)
			{
				line=str_del(line,current_x-1);
				--current_x;
			}
		}
		else if(c==27)
		{
			c=getc();
			if(c=='[')
			{
				c=getc();
				if(c=='D')
				{
					if(current_x)
					{
						--current_x;
					}
				}
				else if(c=='C')
				{
					if(line&&line[current_x])
					{
						++current_x;
					}
				}
				else if(c=='4')
				{
					c=getc();
					if(c=='~')
					{
						if(line)
						{
							current_x=strlen(line);
						}
					}
				}
				else if(c=='1')
				{
					c=getc();
					if(c=='~')
					{
						current_x=0;
					}
				}
			}
		}
		if(line==NULL)
		{
			current_x=0;
		}
		write(1,"\r[BLFS2] ",9);
		if(line)
		{
			write(1,line,strlen(line));
		}
		write(1," \r",2);
		if(line&&strlen(line)+8>=winsz.col)
		{
			strcpy(buf,"\033[");
			sprinti(buf,(strlen(line)+8)/winsz.col,1);
			strcat(buf,"A");
			write(1,buf,strlen(buf));
		}
		if(current_x+8>=winsz.col)
		{
			strcpy(buf,"\033[");
			sprinti(buf,(current_x+8)/winsz.col,1);
			strcat(buf,"B");
			write(1,buf,strlen(buf));
		}
		if((current_x+8)%winsz.col)
		{
			strcpy(buf,"\033[");
			sprinti(buf,(current_x+8)%winsz.col,1);
			strcat(buf,"C");
			write(1,buf,strlen(buf));
		}

		write(1,"\033[?25h",6);
	}
	if(line)
	{
		l=strlen(line);
		while(l>=winsz.col)
		{
			write(1,"\n",1);
			l-=winsz.col;
		}
	}
	write(1,"\n",1);
	return line;
}
