int if_str_match(char *str)
{
	struct file_pos pos;
	int c;
	memcpy(&pos,&current_pos,sizeof(pos));
	while(*str)
	{
		c=file_getc(&pos);
		if(c==-1)
		{
			return 0;
		}
		if(c!=*(unsigned char *)str)
		{
			return 0;
		}
		if(!file_pos_move_right(&pos))
		{
			return 0;
		}
		++str;
	}
	return 1;
}
void search_forward(char *str)
{
	struct file_pos old_pos,old_view_pos;
	int old_end;
	if(*str==0)
	{
		return;
	}
	memcpy(&old_pos,&current_pos,sizeof(current_pos));
	memcpy(&old_view_pos,&view_pos,sizeof(view_pos));
	old_end=current_pos_end;
	while(cursor_right())
	{
		if(if_str_match(str))
		{
			return;
		}
	}
	memcpy(&current_pos,&old_pos,sizeof(current_pos));
	memcpy(&view_pos,&old_view_pos,sizeof(view_pos));
	current_pos_end=old_end;
}
void search_backward(char *str)
{
	struct file_pos old_pos,old_view_pos;
	int old_end;
	if(*str==0)
	{
		return;
	}
	memcpy(&old_pos,&current_pos,sizeof(current_pos));
	memcpy(&old_view_pos,&view_pos,sizeof(view_pos));
	old_end=current_pos_end;
	while(cursor_left())
	{
		if(if_str_match(str))
		{
			return;
		}
	}
	memcpy(&current_pos,&old_pos,sizeof(current_pos));
	memcpy(&view_pos,&old_view_pos,sizeof(view_pos));
	current_pos_end=old_end;
}
void issue_cmd(void)
{
	unsigned long line;
	char buf[64];
	if(cmd_size<1)
	{
		return;
	}
	if(cmd_buf[0]>='0'&&cmd_buf[0]<='9')
	{
		cmd_buf[32]=0;
		if(cmd_size<32)
		{
			cmd_buf[cmd_size]=0;
		}
		line=0;
		sinputi(cmd_buf,&line);
		current_pos.block=file_head;
		current_pos.off=0;
		current_pos.pos=0;
		memcpy(&view_pos,&current_pos,sizeof(current_pos));
		current_x_refine();
		--line;
		while(line)
		{
			cursor_down();
			current_x_refine();
			--line;
		}
		return;
	}
	if(cmd_buf[0]=='/')
	{
		if(cmd_size<2)
		{
			return;
		}
		memcpy(buf,cmd_buf+1,cmd_size-1);
		buf[cmd_size-1]=0;
		search_forward(buf);
		return;
	}
	if(cmd_buf[0]=='\?')
	{
		if(cmd_size<2)
		{
			return;
		}
		memcpy(buf,cmd_buf+1,cmd_size-1);
		buf[cmd_size-1]=0;
		search_backward(buf);
		return;
	}
}
